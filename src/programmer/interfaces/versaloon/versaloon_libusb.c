#include "app_cfg.h"
#include "app_type.h"
#include "app_io.h"
#include "app_log.h"
#include "app_err.h"

#include "interfaces.h"
#include "versaloon_libusb.h"

#include "usbapi.h"

#define VERSALOON_PRODUCTSTRING_INDEX	2
#define VERSALOON_SERIALSTRING_INDEX	3

#define VERSALOON_PRODUCTSTRING			"Versaloon"

#define VERSALOON_VID					0x0483
#define VERSALOON_PID					0xA038
#define VERSALOON_INP					0x82
#define VERSALOON_OUTP					0x03
#define VERSALOON_IFACE					0x00

static struct libusb_device_handle *versaloon_device_handle = NULL;
static uint32_t versaloon_to;

static vsf_err_t versaloon_usb_comm_init(void)
{
	if (!usb_param_valid())
	{
		usb_set_param(VERSALOON_VID, VERSALOON_PID, VERSALOON_INP,
						VERSALOON_OUTP, VERSALOON_IFACE);
	}
	versaloon_device_handle = find_usb_device(usb_param_vid(),
		usb_param_pid(), usb_param_interface(), VERSALOON_SERIALSTRING_INDEX,
		usb_param_serial(), VERSALOON_PRODUCTSTRING_INDEX,
						VERSALOON_PRODUCTSTRING);
	if (NULL == versaloon_device_handle)
	{
		if (usb_param_serial() != NULL)
		{
			LOG_ERROR("Not found vid=0x%04x,pid = 0x%04x,serial = %s.",
						usb_param_vid(), usb_param_pid(), usb_param_serial());
		}
		else
		{
			LOG_ERROR("Not found vid=0x%04x,pid = 0x%04x.", usb_param_vid(),
						usb_param_pid());
		}
		return VSFERR_FAIL;
	}
	return VSFERR_NONE;
}

static vsf_err_t versaloon_usb_comm_fini(void)
{
	if (versaloon_device_handle != NULL)
	{
		libusb_release_interface(versaloon_device_handle, usb_param_interface());
		libusb_close(versaloon_device_handle);
		versaloon_device_handle = NULL;
	}
	
	return VSFERR_NONE;
}

static void versaloon_usb_comm_set_timeout(uint32_t ms)
{
	versaloon_to = ms;
}

static vsf_err_t versaloon_usb_comm_transact(uint8_t *buffer_out,
						uint16_t out_len, uint8_t *buffer_in, uint16_t *in_len)
{
	int ret, transferred;
	
	ret = libusb_bulk_transfer(versaloon_device_handle, usb_param_epout(),
			(unsigned char *)buffer_out, out_len, &transferred, versaloon_to);
	if (ret || (transferred != out_len))
	{
		LOG_ERROR(ERRMSG_FAILURE_OPERATION, "send usb data");
		return ERRCODE_FAILURE_OPERATION;
	}
	
	if (in_len != NULL)
	{
		ret = libusb_bulk_transfer(versaloon_device_handle, usb_param_epin(),
			(unsigned char *)buffer_in, *in_len, &transferred, versaloon_to);
		if (!ret)
		{
			*in_len = (uint16_t)transferred;
			return VSFERR_NONE;
		}
		else
		{
			LOG_ERROR(ERRMSG_FAILURE_OPERATION, "receive usb data");
			return VSFERR_FAIL;
		}
	}
	else
	{
		return VSFERR_NONE;
	}
}

static vsf_err_t versaloon_usb_comm_display_all(void)
{
	// usb parameter
	if (!usb_param_valid())
	{
		usb_set_param(VERSALOON_VID, VERSALOON_PID, VERSALOON_INP,
						VERSALOON_OUTP, 1);
	}
	
	PRINTF(_GETTEXT("Supported Programmer by Versaloon driver:\n"));
	return print_usb_devices(usb_param_vid(), usb_param_pid(),
					VERSALOON_SERIALSTRING_INDEX, usb_param_serial(),
					VERSALOON_PRODUCTSTRING_INDEX, VERSALOON_PRODUCTSTRING);
}

struct interfaces_comm_t versaloon_usb_comm =
{
	versaloon_usb_comm_init,
	versaloon_usb_comm_fini,
	versaloon_usb_comm_set_timeout,
	versaloon_usb_comm_transact,
	
	versaloon_usb_comm_display_all
};
