#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(can);

#define CAN_NODE		DT_ALIAS(mycan)
static const struct device *const can = DEVICE_DT_GET(CAN_NODE);

void can_loop() {
  while (true) {
    if (!device_is_ready(can)) {
      LOG_INF("can device is not ready");
    }

    break;

    k_msleep(1000);
  }
  LOG_INF("can ready");

  int err = can_start(can);
  LOG_INF("can start err %d", err);
  
  while (true) {
    LOG_INF("can loop");
    k_msleep(1000);
  }
}

K_THREAD_DEFINE(t_can,
                1024,
                &can_loop,
                0,0,0,
                5,
                0,
                1000);
