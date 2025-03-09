#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/zbus/zbus.h>
#include <zephyr/drivers/can.h>

#include "can.h"

ZBUS_CHAN_DECLARE(zbus_can);

#define CAN_SOURCE_RIO 1
#define CAN_SOURCE_TEAM 2

struct zbus_can_msg {
    uint8_t source;

    union {
        uint8_t data[CAN_MAX_DLEN];
        struct rio_heartbeat rio;
        struct manipulator_state manipulator;
    };
};

#ifdef __cplusplus
}
#endif