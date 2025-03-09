#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>

#include "zbus_can.h"

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(can);

#define CAN_NODE DT_ALIAS(mycan)
static const struct device *const can = DEVICE_DT_GET(CAN_NODE);

#define CANID_TYPE(id) ((id >> 24) & 0x1f)
#define CANID_TYPE_ROBOT 1

#define CANID_MFG(id) ((id >> 16) & 0xff)
#define CANID_MFG_NI 1

#define CANID_API(id) ((id >> 10) & 0x3f)
#define CANID_INDEX(id) ((id >> 6) & 0xf)
#define CANID_DEVICE(id) (id & 0x3f)

#define CANID_MAKE(type, mfg, api, idx, no)      \
  ((type & 0x1f) << 24) | ((mfg & 0xff) << 16) | \
      ((api & 0x3f) << 10) | ((idx & 0xf) << 6) | (no & 0x3f)

void rx_led(const struct device *dev, struct can_frame *frame, void *user_data)
{
  int api = CANID_API(frame->id);
  int idx = CANID_INDEX(frame->id);
  int dvc = CANID_DEVICE(frame->id);

  struct zbus_can_msg msg = { 
      .source = CAN_SOURCE_TEAM
  };
  strncpy(msg.data, frame->data, frame->dlc);
  int pub = zbus_chan_pub(&zbus_can, &msg, K_NO_WAIT);
  /*
  LOG_INF("led rx, api=%d, idx=%d, dev=%d, len=%d, flags=%d",
          api, idx, dvc,
          frame->dlc, frame->flags);
          */
}

void rx_rio(const struct device *dev, struct can_frame *frame, void *user_data)
{
  int api = CANID_API(frame->id);
  int idx = CANID_INDEX(frame->id);
  int dvc = CANID_DEVICE(frame->id);

  /*
  LOG_INF("rio rx, api=%d, idx=%d, dev=%d, len=%d, flags=%d",
          api, idx, dvc,
          frame->dlc, frame->flags);
          */
  struct zbus_can_msg msg = {
      .source = CAN_SOURCE_RIO
   };
  strncpy(msg.data, frame->data, frame->dlc);
  int pub = zbus_chan_pub(&zbus_can, &msg, K_NO_WAIT);
}

void can_loop()
{
  while (true)
  {
    if (!device_is_ready(can))
    {
      LOG_INF("can device is not ready");
    }

    break;

    k_msleep(1000);
  }
  LOG_INF("can ready");

  int err = can_set_bitrate(can, 1000000);
  LOG_INF("bitrate %d", err);

  struct can_filter filter_robo = {
      CANID_MAKE(CANID_TYPE_ROBOT, CANID_MFG_NI, 0, 0, 0),
      CANID_MAKE(0x1f, 0xff, 0, 0, 0),
      CAN_FILTER_IDE};

  struct can_filter filter_team = {
      CANID_MAKE(10, 8, 0, 0, 0),
      CANID_MAKE(0x1f, 0xff, 0, 0, 0),
      //    CANID_MAKE(0, 0, 0, 0, 0),
      CAN_FILTER_IDE};

  err = can_start(can);
  LOG_INF("start %d", err);

  err = can_add_rx_filter(can,
                          &rx_rio,
                          "roborio",
                          &filter_robo);
  LOG_INF("filter %d", err);

  err = can_add_rx_filter(can,
                          &rx_led,
                          "team",
                          &filter_team);
  LOG_INF("filter %d", err);

  struct can_bus_err_cnt err_cnt;
  while (true)
  {
    enum can_state state;
    err = can_get_state(can, &state, &err_cnt);
    if (err != 0)
    {
      printf("Failed to get CAN controller state: %d", err);
    }
    k_msleep(1000);
  }
}

K_THREAD_DEFINE(t_can,
                1024,
                &can_loop,
                0, 0, 0,
                5,
                0,
                1000);

ZBUS_CHAN_DEFINE(zbus_can,
                 struct zbus_can_msg,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS_EMPTY,
                 {});
