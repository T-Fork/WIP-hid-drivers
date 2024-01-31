/*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* HID driver for SimFeel Pedal
*
* Copyright (C) 2024 Henrik Vestin <henrikvestin@gmail.com>
* Credit goes to Marcus Folkesson <marcus.folkesson@gmail.com> since I'm
* copying his work on hid-vrc2.c
*/

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>

/* Vendor and device ID doesn't seem to be registered properly,
thus "borrowed" from whatever chip maker they used. Keeping these local and will not attempt to populate hid-ids.h. */
#define USB_VENDOR_ID_SIMFEEL_PEDALS 0x10c4
#define USB_DEVICE_ID_SIMFEEL_PEDALS 0x8ccc

static __u8 sfp_rdesc_fixed[] = {
	0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
	0x09, 0x04,        // Changed to Joystick instead of Gamepad
	0xA1, 0x01,        // 	Collection (Application)
	0x09, 0x01,        // 	Usage (Pointer)
	0xA1, 0x00,        // 	Collection (Physical)
	0x05, 0x01,        // 	  Usage Page (Generic Desktop Ctrls)
	0x09, 0x30,        // 	  Usage (X)
	0x09, 0x31,        // 	  Usage (Y)
	0x09, 0x32,        // 	  Usage (Z)
	0x15, 0x00,        // 	  Logical Minimum (0)
	0x26, 0xFF, 0x0F,  // 	  Logical Maximum (4095)
	0x95, 0x03,        // 	  Report Count (3)
	0x75, 0x10,        // 	  Report Size (16)
	0x81, 0x02,        // 	  Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	//0x05, 0x01,        //	    Usage Page (Generic Desktop Ctrls)
	0x05, 0x09,        // 	  Usage Page (Button)
	0x19, 0x01,        // 	  Usage IdMin (Button 1[1])
	0x29, 0x01,        //     UsageIdMax(Button 1[1])
	0x15, 0x00,        // 	  Logical Minimum (0)
	0x25, 0x01,        // 	  Logical Maximum (1)
	0x75, 0x01,        // 	  Report Size (1)
	0x96, 0x80, 0x00,  // 	  Report Count (1)
	0x81, 0x02,        // 	  Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
	0xC0,              // End Collection

// 31 bytes
};

static __u8 *sfp_report_fixup(struct hid_device *hdev, __u8 *rdesc,
				unsigned int *rsize)
{
	hid_info(hdev, "fixing up SimFeel Pedal report descriptor\n");
	*rsize = sizeof(sfp_rdesc_fixed);
	return sfp_rdesc_fixed;w
}

static int sfp_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret;

	/*
	 * If multiple USB endpoints are reported then check that the one correct is parsed
	 * and ignore the one with report incorrect descriptor size.
	 */
	if (hdev->dev_rsize == 23)
		return -ENODEV;

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "parse failed\n");
		return ret;
	}

	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		return ret;
	}

	return 0;
}

static const struct hid_device_id sfp_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_SIMFEEL_PEDALS, USB_DEVICE_ID_SIMFEEL_PEDALS) },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(hid, sfp_devices);

static struct hid_driver sfp_driver = {
	.name = "sfp",
	.id_table = sfp_devices,
	.report_fixup = sfp_report_fixup,
	.probe = sfp_probe,
};
module_hid_driver(sfp_driver);

MODULE_AUTHOR("Henrik Vestin <henrikvestin@gmail.com>");
MODULE_DESCRIPTION("HID driver for SimFeel Pedals");
MODULE_LICENSE("GPL");
