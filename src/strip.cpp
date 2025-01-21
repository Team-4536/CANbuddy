#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/led_strip.h>

#include <pixlib/generator.h>

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(strip);

using namespace px;
