#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/can.h>

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(can);

#define CAN_NODE		DT_ALIAS(can)
static const struct device *const can = DEVICE_DT_GET(CAN_NODE);

