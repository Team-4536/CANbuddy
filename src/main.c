/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define LOG_LEVEL LOG_LEVEL_DBG
LOG_MODULE_REGISTER(main);

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   200

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)

#define WATCHDOG_NODE DT_ALIAS(watchdog)
const struct device *watchdog = DEVICE_DT_GET(WATCHDOG_NODE);

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

        if (!device_is_ready(watchdog)) {
          LOG_ERR("watchdog not ready");
        }

        const struct wdt_timeout_cfg wdt_conf = {
          .flags = WDT_FLAG_RESET_SOC,
          .window.max = 300
        };

        int wdt_chan = wdt_install_timeout(watchdog, &wdt_conf);
        LOG_INF("install wdt = %d", ret);
        ret = wdt_setup(watchdog, 0);
        LOG_INF("wdt setup = %d", ret);

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
          LOG_ERR("failed to confiure status LED");
	}
	ret = gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
	ret = gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);

	while (true) {
          ret = wdt_feed(watchdog, wdt_chan);
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
                  LOG_ERR("failed to toggle status LED");
		}

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
