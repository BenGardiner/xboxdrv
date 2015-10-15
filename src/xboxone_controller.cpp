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

#include "xboxone_controller.hpp"

#include <boost/lambda/lambda.hpp>

#include "chatpad.hpp"
#include "headset.hpp"
#include "helper.hpp"
#include "options.hpp"
#include "raise_exception.hpp"
#include "unpack.hpp"
#include "usb_helper.hpp"

XboxOneController::XboxOneController(libusb_device* dev,
                                     bool try_detach) :
  USBController(dev),
  dev_type(),
  endpoint_in(1),
  endpoint_out(2),
  m_rumble_left(0),
  m_rumble_right(0)
{
  // find endpoints
  endpoint_in  = usb_find_ep(LIBUSB_ENDPOINT_IN,  LIBUSB_CLASS_VENDOR_SPEC, 71, 208, /*only match the zeroth interface */ boost::lambda::_1 == 0);
  endpoint_out = usb_find_ep(LIBUSB_ENDPOINT_OUT, LIBUSB_CLASS_VENDOR_SPEC, 71, 208, /*only match the zeroth interface */ boost::lambda::_1 == 0);

  log_debug("EP(IN):  " << endpoint_in);
  log_debug("EP(OUT): " << endpoint_out);

  usb_claim_interface(0, try_detach);

  uint8_t initcmd[] = { 0x05, 0x20 };
  usb_write(endpoint_out, initcmd, sizeof(initcmd));

  usb_submit_read(endpoint_in, 64);
}

XboxOneController::~XboxOneController()
{
}

void
XboxOneController::set_rumble_real(uint8_t left, uint8_t right)
{
  //TODO
}

void
XboxOneController::set_led_real(uint8_t status)
{
  //TODO
}

bool
XboxOneController::parse(uint8_t* data, int len, XboxGenericMsg* msg_out)
{
  if (len == 0)
  {
    // happens with the XboxOne controller every now and then, just
    // ignore, seems harmless, so just ignore
    //log_debug("zero length read");
  }
  else if (len == 6 && data[0] == 0x07)
  {
    msg_out->type = XBOX_MSG_XBOX360;
    Xbox360Msg& msg = msg_out->xbox360;

    //TODO: don't set type and length
    msg.type = data[0];
    msg.length = data[1];

    msg.guide = unpack::bit(data+4, 0);

    return true;
  }
  else if (len == 18 && data[0] == 0x20)
  {
    msg_out->type = XBOX_MSG_XBOX360;
    Xbox360Msg& msg = msg_out->xbox360;

    msg.type   = data[0];
    msg.length = data[1];

    msg.dpad_up    = unpack::bit(data+5, 0);
    msg.dpad_down  = unpack::bit(data+5, 1);
    msg.dpad_left  = unpack::bit(data+5, 2);
    msg.dpad_right = unpack::bit(data+5, 3);

    msg.start   = unpack::bit(data+4, 2);
    msg.back    = unpack::bit(data+4, 3);
    msg.thumb_l = unpack::bit(data+5, 6);
    msg.thumb_r = unpack::bit(data+5, 7);

    msg.lb     = unpack::bit(data+5, 4);
    msg.rb     = unpack::bit(data+5, 5);

    msg.a = unpack::bit(data+4, 4);
    msg.b = unpack::bit(data+4, 5);
    msg.x = unpack::bit(data+4, 6);
    msg.y = unpack::bit(data+4, 7);

    //TODO: decode the whole 10bit range into a new AXIS
    msg.lt = unpack::int16le(data+6)>>2;
    //TODO: decode the whole 10bit range into a new AXIS
    msg.rt = unpack::int16le(data+8)>>2;

    msg.x1 = unpack::int16le(data+10);
    msg.y1 = unpack::int16le(data+12);

    msg.x2 = unpack::int16le(data+14);
    msg.y2 = unpack::int16le(data+16);

    return true;
  }
  else
  {
    log_debug("unknown: " << raw2str(data, len));
  }

  return false;
}

/* EOF */
