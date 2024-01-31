/*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* HID driver for VKB-Sim Gladiator NXT EVO SC Premium Right
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
#define VKBSIM_VENDOR_ID 0x231d
#define VKBSIM_GLADIATOR_ESC_PR_DEVICE_ID 0x0200

/* This device has 1 analog mini stick with center push (8-way), but is interpreted wrong by HID. Only reports full throw */
static __u8 gnescpr_rdesc_fixed[] = {
	0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
	0x09, 0x04,        // Usage (Joystick)
	0xA1, 0x01,        // Collection (Application)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x85, 0x01,        //   Report ID (1)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x30,        //   Usage (X)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x0F,  //   Logical Maximum (4095)
	0x46, 0xFF, 0x0F,  //   Physical Maximum (4095)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x31,        //   Usage (Y)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x0F,  //   Logical Maximum (4095)
	0x46, 0xFF, 0x0F,  //   Physical Maximum (4095)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x35,        //   Usage (Rz)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x07,  //   Logical Maximum (2047)
	0x46, 0xFF, 0x07,  //   Physical Maximum (2047)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x32,        //   Usage (Z)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x07,  //   Logical Maximum (2047)
	0x46, 0xFF, 0x07,  //   Physical Maximum (2047)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x33,        //   Usage (Rx)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x03,  //   Logical Maximum (1023)
	0x46, 0xFF, 0x03,  //   Physical Maximum (1023)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x34,        //   Usage (Ry)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x03,  //   Logical Maximum (1023)
	0x46, 0xFF, 0x03,  //   Physical Maximum (1023)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x00,        //   Usage Page (Undefined)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x07,  //   Logical Maximum (2047)
	0x46, 0xFF, 0x07,  //   Physical Maximum (2047)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x00,        //   Usage Page (Undefined)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x07,  //   Logical Maximum (2047)
	0x46, 0xFF, 0x07,  //   Physical Maximum (2047)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x09,        //   Usage Page (Button)
	0x19, 0x01,        //   Usage Minimum (0x01)
	0x2A, 0x80, 0x00,  //   Usage Maximum (0x80)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x96, 0x80, 0x00,  //   Report Count (128)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x39,        //   Usage (Hat switch)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0x07, 0x00,  //   Logical Maximum (7)
	0x35, 0x00,        //   Physical Minimum (0)
	0x46, 0x68, 0x01,  //   Physical Maximum (360)
	0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
	0x55, 0x01,        //   Unit Exponent (1)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
	0x09, 0x00,        //   Usage (Undefined)
	0x65, 0x00,        //   Unit (None)
	0x55, 0x00,        //   Unit Exponent (0)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x03,        //   Report Count (3)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x10,        //   Report Size (16)
	0x95, 0x01,        //   Report Count (1)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x17,        //   Report Count (23)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x0B,        //   Report ID (11)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x3F,        //   Report Count (63)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x0C,        //   Report ID (12)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x3F,        //   Report Count (63)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x08,        //   Report ID (8)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x00,        //   Usage (Undefined)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x3F,        //   Report Count (63)
	0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x00,  //   Logical Maximum (255)
	0x46, 0xFF, 0x00,  //   Physical Maximum (255)
	0x85, 0x58,        //   Report ID (88)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x3F,        //   Report Count (63)
	0x09, 0x00,        //   Usage (Undefined)
	0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x85, 0x59,        //   Report ID (89)
	0x75, 0x08,        //   Report Size (8)
	0x95, 0x80,        //   Report Count (-128)
	0x09, 0x00,        //   Usage (Undefined)
	0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0xC0,              // End Collection

// 313 bytes
};

static __u8 *gnescpr_report_fixup(struct hid_device *hdev, __u8 *rdesc,
				unsigned int *rsize)
{
	hid_info(hdev, "fixing up VKB-Sim Gladiator NXT EVO SC Premium Right report descriptor\n");
	*rsize = sizeof(gnescpr_rdesc_fixed);
	return gnescpr_rdesc_fixed;
}

static int gnescpr_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret;

	/*
	 * If multiple USB endpoints are reported then check that the one correct is parsed
	 * and ignore the one with report incorrect descriptor size. E.g. 23
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

static const struct hid_device_id gnescpr_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_gnescpr, USB_DEVICE_ID_gnescpr) },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(hid, gnescpr_devices);

static struct hid_driver gnescpr_driver = {
	.name = "gnescpr",
	.id_table = gnescpr_devices,
	.report_fixup = gnescpr_report_fixup,
	.probe = gnescpr_probe,
};
module_hid_driver(gnescpr_driver);

MODULE_AUTHOR("Henrik Vestin <henrikvestin@gmail.com>");
MODULE_DESCRIPTION("HID driver for VKB-Sim Gladiator EVO R");
MODULE_LICENSE("GPL");
