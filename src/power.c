/*
 * Copyright (c) 2018 Nordic Semiconductor ASA.
 * Copyright (c) 2019 Atmark Techno, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */



#include <clock_control.h>
#include <gpio.h>
#include <power.h>

#include <nrfx_usbd.h>
#include <nrfx/hal/nrf_radio.h>

void device_power(bool enable)
{
	struct device *gpio0 = device_get_binding(DT_GPIO_P0_DEV_NAME);
	struct device *gpio1 = device_get_binding(DT_GPIO_P1_DEV_NAME);

	if (enable) {
		gpio_pin_configure(gpio0, 26, GPIO_DIR_OUT|GPIO_PUD_PULL_DOWN);
		gpio_pin_write(gpio0, 26, 0);
		gpio_pin_configure(gpio1, 2, GPIO_DIR_OUT|GPIO_PUD_PULL_UP);
		gpio_pin_write(gpio1, 2, 1);
		gpio_pin_configure(gpio1, 6, GPIO_DIR_OUT|GPIO_PUD_PULL_UP);
		gpio_pin_write(gpio1, 6, 1);
		sys_pm_resume_devices();
		nrf_usbd_enable();
		nrfx_usbd_wakeup_req();
		nrf_radio_task_trigger(NRF_RADIO_TASK_TXEN);
	} else {
		NRF_RADIO->SHORTS = 0;
		nrf_radio_task_trigger(NRF_RADIO_TASK_DISABLE);
		nrfx_usbd_suspend();
		nrf_usbd_disable();
		sys_pm_suspend_devices();
		gpio_pin_configure(gpio0, 26, GPIO_DIR_OUT|GPIO_PUD_PULL_UP);
		gpio_pin_write(gpio0, 26, 1);
		gpio_pin_configure(gpio1, 2, GPIO_DIR_OUT|GPIO_PUD_PULL_DOWN);
		gpio_pin_write(gpio1, 2, 0);
		gpio_pin_configure(gpio1, 6, GPIO_DIR_OUT|GPIO_PUD_PULL_DOWN);
		gpio_pin_write(gpio1, 6, 0);
	}
}

void sys_pm_notify_power_state_entry(enum power_states state)
{
	switch (state) {
	case SYS_POWER_STATE_SLEEP_2:
		device_power(false);
		k_cpu_idle();
		break;

	case SYS_POWER_STATE_SLEEP_3:
		device_power(false);
		k_cpu_idle();
		break;

	case SYS_POWER_STATE_DEEP_SLEEP_1:
		device_power(false);
		break;

	default:
		break;
	}
}

void sys_pm_notify_power_state_exit(enum power_states state)
{
	switch (state) {
	case SYS_POWER_STATE_SLEEP_2:
		device_power(true);
		break;

	case SYS_POWER_STATE_SLEEP_3:
		device_power(true);
		break;

	case SYS_POWER_STATE_DEEP_SLEEP_1:
		device_power(true);
		break;

	default:
		break;
	}
}
