/* SPDX-License-Identifier: GPL-2.0 */
/*
 * sensor driver adapter
 *
 * Copyright (C) 2022 Rockchip Electronics Co., Ltd.
 *
 */

#ifndef _SENSOR_ADAPTER_H
#define _SENSOR_ADAPTER_H

#include <linux/types.h>

int rkcam_sensor_enable_mclk(u8 i2cdev, u32 mclk_index, u32 mclk_rate);
int rkcam_sensor_disable_mclk(u8 i2cdev, u32 mclk_index);

#endif /* _SENSOR_ADAPTER_H */
