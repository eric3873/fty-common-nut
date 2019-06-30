/*  =========================================================================
    fty_common_nut_scan - class description

    Copyright (C)
        2014 - 2018 Eaton
        2019        Arnaud Quette <arnaud.quette@free.fr>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

/*
@header
    fty_common_nut_scan -
@discuss
@end
*/

#ifndef FTY_COMMON_NUT_SCAN_H_INCLUDED
#define FTY_COMMON_NUT_SCAN_H_INCLUDED

namespace nutcommon {

struct ScanRangeOptions
{
    ScanRangeOptions(const std::string& ip, int timeout) :
        ip_address_start(ip), ip_address_end(ip), timeout(timeout) {}

    ScanRangeOptions(const std::string& start, const std::string& end, int timeout) :
        ip_address_start(start), ip_address_end(end), timeout(timeout) {}

    std::string ip_address_start;
    std::string ip_address_end;
    int timeout;
};

int scanDeviceRangeSNMPv3(
    const ScanRangeOptions& scanOptions,
    const CredentialsSNMPv3& credentials,
    bool use_dmf,
    DeviceConfigurations& out);

int scanDeviceRangeSNMPv1(
    const ScanRangeOptions& scanOptions,
    const CredentialsSNMPv1& credentials,
    bool use_dmf,
    DeviceConfigurations& out);

int scanDeviceRangeNetXML(
    const ScanRangeOptions& scanOptions,
    DeviceConfigurations& out);

int scanDeviceRangeModbusTCP(
    const ScanRangeOptions& scanOptions,
    DeviceConfigurations& out);

}

#endif
