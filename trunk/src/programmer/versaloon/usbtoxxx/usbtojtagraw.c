/***************************************************************************
 *   Copyright (C) 2009 by Simon Qian <SimonQian@SimonQian.com>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>

#include "../versaloon_include.h"
#include "../versaloon.h"
#include "../versaloon_internal.h"
#include "usbtoxxx.h"
#include "usbtoxxx_internal.h"

uint8_t usbtojtagraw_num_of_interface = 0;

RESULT usbtojtagraw_init(void)
{
	return usbtoxxx_init_command(USB_TO_JTAG_RAW, 
									&usbtojtagraw_num_of_interface);
}

RESULT usbtojtagraw_fini(void)
{
	return usbtoxxx_fini_command(USB_TO_JTAG_RAW);
}

RESULT usbtojtagraw_config(uint8_t interface_index, uint16_t kHz)
{
	uint8_t cfg_buf[2];
	
#if PARAM_CHECK
	if (interface_index > 7)
	{
		LOG_BUG(_GETTEXT("invalid inteface_index %d.\n"), interface_index);
		return ERROR_FAIL;
	}
#endif
	
	cfg_buf[0] = (kHz >> 0) & 0xFF;
	cfg_buf[1] = (kHz >> 8) & 0xFF;
	
	return usbtoxxx_conf_command(USB_TO_JTAG_RAW, interface_index, cfg_buf, 2);
}

RESULT usbtojtagraw_execute(uint8_t interface_index, uint8_t *tdi, 
							uint8_t *tms, uint8_t *tdo, uint16_t bytelen)
{
#if PARAM_CHECK
	if (interface_index > 7)
	{
		LOG_BUG(_GETTEXT("invalid inteface_index %d.\n"), interface_index);
		return ERROR_FAIL;
	}
#endif
	memcpy(versaloon_cmd_buf, tdi, bytelen);
	memcpy(versaloon_cmd_buf + bytelen, tms, bytelen);
	return usbtoxxx_inout_command(USB_TO_JTAG_RAW, interface_index, 
				versaloon_cmd_buf, bytelen * 2, bytelen, tdo, 0, bytelen, 0);
}
