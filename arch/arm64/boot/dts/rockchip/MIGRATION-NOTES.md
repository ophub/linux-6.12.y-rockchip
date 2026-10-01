# Rockchip BSP device tree migration 6.1 -> 6.12 notes

This file records which device trees were migrated from the 6.1 BSP tree and
which ones had to be deferred, together with the reasons.  It is a plain
documentation file, it is not referenced by the build.

## Deferred boards

| board | reason | how to finish it |
|---|---|---|
| `rk3588s-orangepi-5b.dts` | This file (identical in both trees) only adds `&wireless_bluetooth { status = "okay"; }` and `&wireless_wlan { status = "okay"; }` on top of `rk3588s-orangepi-5.dts`. The 6.12 tree ships the *mainline* version of `rk3588s-orangepi-5.dts`, which does not define those wireless regulator nodes at all (the 6.1 BSP version does). Enabling the board would therefore drop the WiFi/Bluetooth power rails, and a faithful port has to bring over the `wireless-bluetooth` / `wireless-wlan` regulator nodes **and** their dependent pinctrl nodes (`bt_gpio`, `wifi_host_wake_irq`, `wifi_poweren_gpio`, `uart9m2_rtsn`, `uart9_gpios`) plus the `hym8563` RTC node. | Port the two regulators and their pinctrl/RTC dependencies from the 6.1 `rk3588s-orangepi-5.dts` into the 6.12 one, then re-add `rk3588s-orangepi-5b.dtb` to the Makefile. |

The source file `rk3588s-orangepi-5b.dts` is already present in the tree, only
its `dtb-` entry is intentionally missing.

## Adaptation rules applied during the migration

The 6.12 tree is authoritative; migrated/source files were only repaired, never
replaced wholesale.  The recurring differences and their fixes:

1. **Test-pattern/pad ops, remove/probe prototypes, `set_fs`, `v4l2_subdev`
   state API** - migrated boards needed the same mechanical updates as the
   drivers, see the individual commits.
2. **Mainline vs vendor node names for the USB3 controllers** - boards written
   against the mainline layout are mapped onto the vendor glue+dwc3 pairs:

   | mainline label | address | vendor glue | vendor dwc3 child |
   |---|---|---|---|
   | `usb_host0_xhci` | `fc000000` | `usbdrd3_0` | `usbdrd_dwc3_0` |
   | `usb_host1_xhci` | `fc400000` | `usbdrd3_1` | `usbdrd_dwc3_1` |
   | `usb_host2_xhci` | `fcd00000` | `usbhost3_0` | `usbhost_dwc3_0` |
   | (rk356x) `usb_host0_xhci` | `fcc00000` | `usbdrd30` | `usbdrd_dwc3` |
   | (rk356x) `usb_host1_xhci` | `fd000000` | `usbhost30` | `usbhost_dwc3` |

   The glue node is enabled with `status = "okay"` and the original properties
   (`dr_mode`, `extcon`, `status`, ...) stay on the dwc3 child.
3. **Renamed PHY/port labels**: `combphy0/1/2` -> `combphy0_us/combphy1_usq/combphy2_psq`,
   `usb2phy1_{host,otg}` -> `u2phy1_{host,otg}`.
4. **`usbdp_phy` sub-ports**: the 6.1 BSP exposed `usbdp_phy_dp` / `usbdp_phy_u3`
   (and `usbdp_phy0/1_{dp,u3}` on rk3588) as child PHYs.  The 6.12 dtsi uses a
   single node selected through `PHY_TYPE_DP` / `PHY_TYPE_USB3`, so the two
   overrides collapse into one `&usbdp_phy { status = "okay"; }` (or are dropped
   when neither sub-port was enabled, since the SoC default is disabled).
5. **Nodes that no longer exist** are dropped, together with any
   `remote-endpoint` that pointed at a label defined inside them: `hdmi_in`,
   `hdmi_out`, `rk809_slppin_*`, `package_thermal` and the duplicated
   `vp0_out_hdmi` endpoint.
6. **dt-bindings headers** that migrated boards need were restored:
   `dt-bindings/phy/jlsemi-dt-phy.h` and the `PCA_IO*` defines of
   `dt-bindings/pinctrl/rockchip.h`.
7. Makefile `dtb-$(CONFIG_ARCH_ROCKCHIP)` entries are always kept in ASCII
   order.

## Verification command

```
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- CC=aarch64-linux-gnu-gcc \
     LD=ld.lld dtbs -j8
```

This is the same `dtbs` step that `/usr/src/make-kernel.sh` runs before the full
kernel build.
