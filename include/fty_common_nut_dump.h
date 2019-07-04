/*  =========================================================================
    fty_common_nut_dump - class description

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
    fty_common_nut_dump -
@discuss
@end
*/

#ifndef FTY_COMMON_NUT_DUMP_H_INCLUDED
#define FTY_COMMON_NUT_DUMP_H_INCLUDED

#include "fty_common_nut_credentials.h"
#include "fty_common_nut_utils.h"

namespace nutcommon {

/**
 * \brief Helper method to get data from a SNMPv3 device.
 *
 * \param[in] port Device to scan (hostname).
 * \param[in] credentials SNMPv3 credentials to use.
 * \param[out] out Map of key/value data returned by driver.
 * \return 0 if success.
 */
int dumpDeviceSNMPv3(
    const std::string& port,
    const CredentialsSNMPv3& credentials,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out);

/**
 * \brief Helper method to get data from a SNMPv1 device.
 *
 * \param[in] port Device to scan (hostname).
 * \param[in] credentials SNMPv1 credentials to use.
 * \param[out] out Map of key/value data returned by driver.
 * \return 0 if success.
 */
int dumpDeviceSNMPv1(
    const std::string& port,
    const CredentialsSNMPv1& credentials,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out);

/**
 * \brief Helper method to get data from a netxml device.
 *
 * \param[in] port Device to scan (hostname).
 * \param[out] out Map of key/value data returned by driver.
 * \return 0 if success.
 */
int dumpDeviceNetXML(
    const std::string& port,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out);

/**
 * \brief Helper method to get data from a Modbus device.
 *
 * \param[in] port Device to scan (hostname).
 * \param[out] out Map of key/value data returned by driver.
 * \return 0 if success.
 */
int dumpDeviceModbus(
    const std::string& port,
    const CredentialsModbus& credentials,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out);

/**
 * \brief Helper method to get data from a SNMPv3 device.
 *
 * \param[in] port Device to scan (NUT device).
 * \param[out] out Map of key/value data returned by driver.
 * \return 0 if success.
 */
int dumpDeviceDummy(
    const std::string& device,
    unsigned loop_nb,
    unsigned loop_iter_time,
    KeyValues& out);

}

#endif
