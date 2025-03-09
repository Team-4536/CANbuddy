#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led_strip.h>

#include "zbus_can.h"

#include <pixlib/generator.h>

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(strip);

using namespace px;

#define UPDATE_PERIOD 20

#define STRIP_NODE DT_ALIAS(strip)

#if DT_NODE_HAS_PROP(STRIP_NODE, chain_length)
#define STRIP_LEN DT_PROP(STRIP_NODE, chain_length)
#else
#error strip chain-len not specified
#endif

ZBUS_MSG_SUBSCRIBER_DEFINE(strip_listener);
ZBUS_CHAN_ADD_OBS(zbus_can, strip_listener, 0);

static struct led_rgb backing[STRIP_LEN];
const struct device *strip = DEVICE_DT_GET(STRIP_NODE);

static void update(Generator *g) {
  size_t size = g->getSize();

  for (index_t i = 0; i < size; i++) {
    Pixel p = g->get(i);
    backing[i] = {p.rgb.red, p.rgb.green, p.rgb.blue};
  }
  int ret = led_strip_update_rgb(strip, backing, size);

  if (ret) {
    LOG_ERR("couldn't update strip: %d", ret);
  }
}

K_TIMER_DEFINE(strip_timer, NULL, NULL);

#define LEFT_SIZE 7
#define RIGHT_SIZE 7
#define TOP_SIZE 2

void strip_loop() {
  if (!device_is_ready(strip)) {
    LOG_INF("strip device is not ready");
  }

  // the entire length of the strip
  auto all = new Stitch(STRIP_LEN);

  // left side
  auto left = new Stack(LEFT_SIZE);
  auto bgl = new Chase(LEFT_SIZE, Pixel::Green());
  // masked left
  auto bglm = new mask::Linear(LEFT_SIZE, bgl, LEFT_SIZE, 0);

  auto top = new Stack(TOP_SIZE);

  auto right = new Stack(RIGHT_SIZE);
  auto bgr = new Chase(RIGHT_SIZE, Pixel::Green());
  auto bgrm = new mask::Linear(RIGHT_SIZE, bgr, RIGHT_SIZE, 0);

  left->push(bglm);
  right->push(bgrm);

  auto strobe = new Solid(TOP_SIZE, Pixel::Green());
  top->push(strobe->withStrobe(20));

  all->push(left);
  all->push(top);
  all->push(right->withReverse());

  uint8_t last_coral = 255;
  uint8_t last_elevator = 0;

k_timer_start(&strip_timer, K_NO_WAIT, K_MSEC(UPDATE_PERIOD));
  while (true) {
    static int updates = 0;

//    LOG_INF("strip loop %d", updates++);

    uint32_t n = k_timer_status_sync(&strip_timer);
    if (n > 1) {
      LOG_INF("%d timer expires", n);
    }

    int zbus_stat = 0;

    const struct zbus_channel *chan;
    struct zbus_can_msg msg;

    do {
      zbus_stat = zbus_sub_wait_msg(&strip_listener,
        &chan,
        &msg,
        K_NO_WAIT);
      if (zbus_stat == 0) {
        switch (msg.source) {
        case CAN_SOURCE_RIO:
          LOG_INF("rio msg, enabled %d, red %d, test %d", msg.rio.enabled, msg.rio.redAlliance, msg.rio.testMode);
          if (msg.rio.redAlliance) {
            bgl->setColor(Pixel::Red());
            bgr->setColor(Pixel::Red());
          } else {
            bgl->setColor(Pixel::Blue());
            bgr->setColor(Pixel::Blue());
          }
          break;
        case CAN_SOURCE_TEAM:
          LOG_INF("team msg %d, %d", msg.manipulator.elevatorPos, msg.manipulator.coralState);
          if (last_coral != msg.manipulator.coralState) {

            last_coral = msg.manipulator.coralState;
          }
          if (last_elevator != msg.manipulator.elevatorPos) {
            last_elevator = msg.manipulator.elevatorPos;
            index_t newEnd = last_elevator * LEFT_SIZE / 100;
            LOG_INF("new end %d", newEnd);
            bglm->setEnd(newEnd);
            bgrm->setEnd(newEnd);
            if (last_elevator <= 1) {
              strobe->enable();
            } else {
              strobe->disable();
            }
          }
          break;
        default:
          LOG_INF("unknown msg");
          break;
        }
      }
    } while (zbus_stat == 0);

    update(all);
    all->next();

    //k_msleep(20);
  }
}

K_THREAD_DEFINE(
    t_strip,
    1024,
    &strip_loop,
    0, 0, 0,
    5,
    0,
    1000);


    /*
ZBUS_CHAN_DEFINE(
    zbus_strip,
    struct zbus_strip_msg,
    NULL,
    NULL,
    ZBUS_OBSERVERS(strip_listener),
    {});
*/
