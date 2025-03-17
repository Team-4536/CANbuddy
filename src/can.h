#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

struct rio_heartbeat {
    uint8_t matchTimeSeconds;
    uint16_t matchNumber:10;
    uint8_t replayNumber:6;
    uint8_t redAlliance:1;
    uint8_t enabled:1;
    uint8_t autonomousMode:1;
    uint8_t testMode:1;
    uint8_t systemWatchdog:1;
};

struct manipulator_state {
    uint8_t elevatorPos;
    uint8_t coralState;
    uint8_t elevatorMode;
    uint8_t selectedSetPoint;
    uint8_t atSetPoint;
    uint8_t chuteState;
};

#ifdef __cplusplus
}
#endif
