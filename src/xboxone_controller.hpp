/*
**  Xbox/XboxOne USB Gamepad Userspace Driver
**  Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
**  Copyright (C) 2015 Ted Mielczarek <ted@mielczarek.org>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef HEADER_XBOXONE_CONTROLLER_HPP
#define HEADER_XBOXONE_CONTROLLER_HPP

#include <libusb.h>
#include <memory>
#include <string>

#include "usb_controller.hpp"

struct XPadDevice;

class XboxOneController : public USBController
{
private:
  XPadDevice*        dev_type;

  int endpoint_in;
  int endpoint_out;

  uint8_t m_rumble_left;
  uint8_t m_rumble_right;

public:
  XboxOneController(libusb_device* dev,
                      bool try_detach);
  ~XboxOneController();

  void set_rumble_real(uint8_t left, uint8_t right);
  void set_led_real(uint8_t status);
  bool parse(uint8_t* data, int len, XboxGenericMsg* msg_out);

private:
  XboxOneController (const XboxOneController&);
  XboxOneController& operator= (const XboxOneController&);
};
#endif

/* EOF */
