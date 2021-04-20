/*  =========================================================================
    fty_common_nut_scan - class description

    Copyright (C) 2014 - 2020 Eaton

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

#include <fty_security_wallet.h>

namespace fty {
namespace nut {

enum ScanProtocol
{
    SCAN_PROTOCOL_NETXML,
    SCAN_PROTOCOL_SNMP,
    SCAN_PROTOCOL_SNMP_DMF
};

/**
 * \brief Scan for NUT driver configurations on an IP address.
 * \param protocol Protocol to scan for.
 * \param idAddress IP address to scan.
 * \param timeout Timeout of scan, in seconds.
 * \param documents Security wallet documents to use for scan (at most one set of credentials can be specified).
 * \return List of device configurations found.
 */
DeviceConfigurations scanDevice(
    ScanProtocol protocol,
    std::string ipAddress,
    unsigned timeout,
    const std::vector<secw::DocumentPtr>& documents = {}
);

/**
 * \brief Scan for NUT configurations on an IP address range.
 * \param protocol Protocol to scan for.
 * \param idAddressStart First IP address to scan.
 * \param idAddressEnd Last IP address to scan.
 * \param timeout Timeout of scan, in seconds.
 * \param documents Security wallet documents to use for scan (at most one set of credentials can be specified).
 * \return List of device configurations found.
 */
DeviceConfigurations scanRangeDevices(
    ScanProtocol protocol,
    std::string ipAddressStart,
    std::string ipAddressEnd,
    unsigned timeout,
    const std::vector<secw::DocumentPtr>& documents = {}
);

}
}

#endif
