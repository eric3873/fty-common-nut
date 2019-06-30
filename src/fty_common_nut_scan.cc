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

#include "fty_common_nut_classes.h"

namespace nutcommon {

int s_scanDeviceRange(
    const ScanRangeOptions& scanOptions,
    const MlmSubprocess::Argv& extra,
    DeviceConfigurations& out)
{
    MlmSubprocess::Argv args = { "nut-scanner", "--quiet", "--disp_parsable", "--start_ip", scanOptions.ip_address_start };
    std::string stdout, stderr;
    int ret;

    if (scanOptions.ip_address_start != scanOptions.ip_address_end) {
        args.emplace_back("--end_ip");
        args.emplace_back(scanOptions.ip_address_end);
    }

    for (const auto& i : extra) {
        args.emplace_back(i);
    }

    ret = priv::runCommand(args, stdout, stderr, scanOptions.timeout);

    auto result = parseScannerOutput(stdout);
    out.insert(out.end(), result.begin(), result.end());

    return ret;
}

int scanDeviceRangeSNMPv3(
    const ScanRangeOptions& scanOptions,
    const CredentialsSNMPv3& credentials,
    bool use_dmf,
    DeviceConfigurations& out)
{
    if (::getenv("BIOS_NUT_USE_DMF")) {
        use_dmf = true;
    }

    MlmSubprocess::Argv extra = {
        use_dmf ? "-z" : "--snmp_scan",
        "--secName", credentials.secName
    };

    /**
     * There are three possible cases:
     *  - noAuthNoPriv (nothing provided)
     *  - authNoPriv (authentification password provided)
     *  - authPriv (authentification and privacy password provided)
     * Both authentification and privacy passwords may optionally provide a specific protocol to use.
     */
    if (!credentials.authPassword.empty()) {
        extra.emplace_back("--authPassword");
        extra.emplace_back(credentials.authPassword);
        if (!credentials.authProtocol.empty()) {
            extra.emplace_back("--authProtocol");
            extra.emplace_back(credentials.authProtocol);
        }

        if (!credentials.privPassword.empty()) {
            extra.emplace_back("--privPassword");
            extra.emplace_back(credentials.privPassword);
            if (!credentials.privProtocol.empty()) {
                extra.emplace_back("--privProtocol");
                extra.emplace_back(credentials.privProtocol);
            }

            extra.emplace_back("--secLevel");
            extra.emplace_back("authPriv");
        }
        else {
            extra.emplace_back("--secLevel");
            extra.emplace_back("authNoPriv");
        }
    }
    else {
        extra.emplace_back("--secLevel");
        extra.emplace_back("noAuthNoPriv");
    }

    return s_scanDeviceRange(scanOptions, extra, out);
}

int scanDeviceRangeSNMPv1(
    const ScanRangeOptions& scanOptions,
    const CredentialsSNMPv1& credentials,
    bool use_dmf,
    DeviceConfigurations& out)
{
    if (::getenv("BIOS_NUT_USE_DMF")) {
        use_dmf = true;
    }

    MlmSubprocess::Argv extra = {
        use_dmf ? "-z" : "--snmp_scan",
        "--community", credentials.community
    };

    return s_scanDeviceRange(scanOptions, extra, out);
}

int scanDeviceRangeNetXML(
    const ScanRangeOptions& scanOptions,
    DeviceConfigurations& out)
{
    MlmSubprocess::Argv extra = { "--xml_scan" };

    return s_scanDeviceRange(scanOptions, extra, out);
}

int scanDeviceRangeModbusTCP(
    const ScanRangeOptions& scanOptions,
    DeviceConfigurations& out)
{
    MlmSubprocess::Argv extra = { "--modbus_tcp_scan" };

    return s_scanDeviceRange(scanOptions, extra, out);
}

}
