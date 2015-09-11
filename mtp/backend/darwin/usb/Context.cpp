/*
 * Android File Transfer for Linux: MTP client for android devices
 * Copyright (C) 2015  Vladimir Menshakov

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <usb/Context.h>
#include <usb/call.h>
#include <stdio.h>

namespace
{

int usb_setup_device_iterator (io_iterator_t *deviceIterator, UInt32 location) {
  CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOUSBDeviceClassName);

  if (!matchingDict)
    return kIOReturnError;

  if (location) {
    CFMutableDictionaryRef propertyMatchDict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                         &kCFTypeDictionaryKeyCallBacks,
                                                                         &kCFTypeDictionaryValueCallBacks);

    if (propertyMatchDict) {
      /* there are no unsigned CFNumber types so treat the value as signed. the os seems to do this
         internally (CFNumberType of locationID is 3) */
      CFTypeRef locationCF = CFNumberCreate (NULL, kCFNumberSInt32Type, &location);

      CFDictionarySetValue (propertyMatchDict, CFSTR(kUSBDevicePropertyLocationID), locationCF);
      /* release our reference to the CFNumber (CFDictionarySetValue retains it) */
      CFRelease (locationCF);

      CFDictionarySetValue (matchingDict, CFSTR(kIOPropertyMatchKey), propertyMatchDict);
      /* release out reference to the CFMutableDictionaryRef (CFDictionarySetValue retains it) */
      CFRelease (propertyMatchDict);
    }
    /* else we can still proceed as long as the caller accounts for the possibility of other devices in the iterator */
  }

  return IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, deviceIterator);
}

}

namespace mtp { namespace usb
{

	Context::Context()
	{
		io_iterator_t deviceIterator;

		USB_CALL(usb_setup_device_iterator (&deviceIterator, 0));

		io_service_t service;
		while ((service = IOIteratorNext (deviceIterator)))
		{
			(void) process_new_device (ctx, service);

			IOObjectRelease(service);
		}

		IOObjectRelease(deviceIterator);
	}

	Context::~Context()
	{ }

	void Context::Wait()
	{ }


}}
