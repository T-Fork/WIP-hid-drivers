/*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* HID driver for Polsimer F74N Formula Wheel
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
#define POLSIMER_VENDOR_ID 0x16c0
#define POLSIMER_F74N_DEVICE_ID 0x0486

/* This device doesn't have any buttons or dials that support axis. */
static __u8 f74n_rdesc_fixed[] = {
	0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
	//0x09, 0x04,        // Usage (Joystick) **Changed to 0x05 Game Pad
    0x09, 0x05,        // Usage (Game Pad) 
	0xA1, 0x01,        // Collection (Application)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x01,        //   Logical Maximum (1)
	0x75, 0x01,        //   Report Size (1)
	0x95, 0x20,        //   Report Count (32)
	0x05, 0x09,        //   Usage Page (Button)
	0x19, 0x01,        //   Usage Minimum (0x01)
	0x29, 0x20,        //   Usage Maximum (0x20)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x15, 0x00,        //   Logical Minimum (0)
	0x25, 0x07,        //   Logical Maximum (7)
	0x35, 0x00,        //   Physical Minimum (0)
	0x46, 0x3B, 0x01,  //   Physical Maximum (315)
	0x75, 0x04,        //   Report Size (4)
	0x95, 0x01,        //   Report Count (1)
	0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
	0x09, 0x39,        //   Usage (Hat switch)
	0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
//	0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
//	0x09, 0x01,        //   Usage (Pointer)
//	0xA1, 0x00,        //   Collection (Physical)
//	0x15, 0x00,        //     Logical Minimum (0)
//	0x26, 0xFF, 0x03,  //     Logical Maximum (1023)
//	0x75, 0x0A,        //     Report Size (10)
//	0x95, 0x04,        //     Report Count (4)
//	0x09, 0x30,        //     Usage (X)
//	0x09, 0x31,        //     Usage (Y)
//	0x09, 0x32,        //     Usage (Z)
//	0x09, 0x35,        //     Usage (Rz)
//	0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              //   End Collection
	0x15, 0x00,        //   Logical Minimum (0)
	0x26, 0xFF, 0x03,  //   Logical Maximum (1023)
	0x75, 0x0A,        //   Report Size (10)
	0x95, 0x02,        //   Report Count (2)
	0x09, 0x36,        //   Usage (Slider)
	0x09, 0x36,        //   Usage (Slider)
	0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,              // End Collection

// 85 bytes
};

static __u8 *f74n_report_fixup(struct hid_device *hdev, __u8 *rdesc,
				unsigned int *rsize)
{
	hid_info(hdev, "fixing up Polsimer F74N Formula Wheel report descriptor\n");
	*rsize = sizeof(f74n_rdesc_fixed);
	return f74n_rdesc_fixed;
}

static int f74n_probe(struct hid_device *hdev, const struct hid_device_id *id)
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

static const struct hid_device_id f74n_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_f74n, USB_DEVICE_ID_f74n) },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(hid, f74n_devices);

static struct hid_driver f74n_driver = {
	.name = "f74n",
	.id_table = f74n_devices,
	.report_fixup = f74n_report_fixup,
	.probe = f74n_probe,
};
module_hid_driver(f74n_driver);

MODULE_AUTHOR("Henrik Vestin <henrikvestin@gmail.com>");
MODULE_DESCRIPTION("HID driver for Polsimer F74N Formula Wheel");
MODULE_LICENSE("GPL");
