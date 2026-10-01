# SPDX-License-Identifier: GPL-2.0
#
# Device tree overlays for Rockchip boards.
#
# Overlay sources use the .dtso suffix (the 6.1 BSP used .dts, but the
# mainline kbuild rules that the 6.12 tree carries only build %.dtbo from
# %.dtso).  The resulting blobs are applied on top of a base .dtb at runtime,
# which requires the base DTB to carry __symbols__ - see the DTC_FLAGS line in
# the parent Makefile.

dtb-$(CONFIG_ARCH_ROCKCHIP) += rk3588-hdmirx.dtbo
dtb-$(CONFIG_ARCH_ROCKCHIP) += rk3588-pwm0-m1.dtbo

targets += $(dtbo-y)
clean-files := *.dtbo
