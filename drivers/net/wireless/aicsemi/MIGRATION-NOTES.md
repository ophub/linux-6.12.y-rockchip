# AIC8800 driver migration 6.18 -> 6.12 notes

The AICSemi aic8800 WLAN driver and the accompanying AIC8800 Bluetooth
drivers were taken over verbatim from the 6.18 BSP.  Only the API usages
listed below had to be adapted for 6.12.

## What was migrated

| Part              | Location                                       | Module              | Kconfig symbol              |
| ----------------- | ---------------------------------------------- | ------------------- | --------------------------- |
| WLAN SDIO         | `aic8800/sdio/fdrv`, `aic8800/sdio/bsp`        | `aic8800_sdio_wlan.ko`, `aic8800_sdio_bsp.ko` | `CONFIG_AIC8800_SDIO_WLAN` |
| WLAN USB          | `aic8800/usb/fdrv`, `aic8800/usb/bsp`          | `aic8800_usb_wlan.ko`, `aic8800_usb_bsp.ko`   | `CONFIG_AIC8800_USB_WLAN`  |
| Bluetooth USB     | `drivers/bluetooth/aic_btusb`                  | `aic8800_btusb.ko`  | `CONFIG_AIC8800_BTUSB_SUPPORT` |
| Bluetooth UART    | `drivers/bluetooth/hci_aic8800.c`              | `hci_aic8800.ko`    | `CONFIG_BT_HCIAIC`          |

`CONFIG_USB_BT = y` in `aic8800/usb/{fdrv,bsp}/Makefile` additionally builds
the vendor Bluetooth glue (`aic_bluetooth_main.c`, `aicbluetooth*.c`) into
`aic8800_usb_bsp.ko`, exactly as the 6.18 BSP does.

## WLAN: cfg80211 API differences between 6.12 and 6.18

`sdio/fdrv/rwnx_compat.h` and `usb/fdrv/rwnx_defs.h` gain two helpers:

* `timer_container_of()` is not available before 6.16, so it is mapped back
  to `from_timer()`;
* `RWNX_CFG80211_WDEV_API` selects the post-6.17 cfg80211 flavour the vendor
  driver is written against (0 on 6.12).

The guarded differences are:

* the key/station callbacks (`add_key`, `get_key`, `del_key`,
  `add_station`, `del_station`, `change_station`, ...) take a
  `struct net_device` instead of a `struct wireless_dev`;
* `set_monitor_channel()` is called without the leading `struct net_device`
  argument (added in 6.13);
* `set_wiphy_params()`, `set_tx_power()` and `get_tx_power()` have no
  `radio_idx`/`link_id` argument (added in 6.17, the same threshold the
  vendor uses in its own guards);
* the remain-on-channel, `mgmt_tx` and `probe_client` cookies are out
  parameters, so the driver hands the cookie out instead of receiving it;
* `cfg80211_new_sta()`, `cfg80211_del_sta()`, `cfg80211_probe_status()` and
  the `cfg80211_rx_spurious_frame()`/`cfg80211_rx_unexpected_4addr_frame()`
  helpers use the pre-6.17 argument list;
* the 6.12 layout of `struct ieee80211_mgmt.u.action` is used for the TDLS
  discover response.

Verification: `make drivers/net/wireless/aicsemi/` -> 0 warnings, 0 errors.

## Bluetooth

The vendor sources are byte-identical to the ones shipped in the 6.18 BSP
(and to the 6.1 BSP copies).  Only `hci_aic8800.c` needed one change: before
6.17 `h4_recv_buf()` takes a `struct hci_dev` rather than the
`struct hci_uart`, and `int` rather than `size_t` arguments.

Verification: with `CONFIG_AIC8800_BTUSB_SUPPORT=m` and `CONFIG_BT_HCIAIC=m`,
`make modules` builds `aic8800_btusb.ko` and `hci_aic8800.ko` with no new
warnings.

## SDIO Bluetooth is intentionally disabled

`CONFIG_SDIO_BT` in `sdio/fdrv/Makefile` stays `n`, as in both the 6.1 and
the 6.18 BSP.  Do not enable it on a kernel that uses the in-tree Bluetooth
stack:

* `aic_btsdio.c` implements its own HCI core (`hci_alloc_dev()`,
  `hci_free_dev()`, `hci_register_dev()`, `hci_unregister_dev()`,
  `hci_recv_frame()`, `hci_recv_fragment()`, ... are all private static
  functions) and `aic_btsdio.h` carries private copies of
  `hci.h`/`hci_core.h` (`struct hci_dev`, `struct hci_conn_hash`,
  `struct hci_dev_stats`, `bdaddr_t`, `bt_cb()`, the `HCI_UP` enum, ...).
* The WLAN headers included by the same translation unit already pull in
  `<net/bluetooth/hci_core.h>`, so the two definitions collide: building
  with `CONFIG_SDIO_BT=y` produces about 60 "redefinition / conflicting
  types" errors.
* The code talks to the vendor's own HCI stack (the `/dev/aicbt_dev`
  character device, the `CONFIG_BLUEDROID` ABI and vendor SDIO BT firmware)
  instead of the kernel Bluetooth subsystem, so reviving it is a rewrite of
  the vendored HCI core, not an API port.

For Bluetooth use `aic8800_btusb.ko` (USB) or `hci_aic8800.ko` (UART) over
the in-tree Bluetooth stack.
