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

#include "fty_common_nut_classes.h"

namespace fty {
namespace nut {

static std::map<ScanProtocol, std::string> s_scanProtocols {
    { SCAN_PROTOCOL_NETXML,     "--xml_scan" },
    { SCAN_PROTOCOL_SNMP,       "--snmp_scan" },
    { SCAN_PROTOCOL_SNMP_DMF,   "--snmp_scan_dmf" },
};

static std::map<ScanProtocol, std::string> s_driverProtocols {
    { SCAN_PROTOCOL_NETXML,     "netxml-ups" },
    { SCAN_PROTOCOL_SNMP,       "snmp-ups" },
    { SCAN_PROTOCOL_SNMP_DMF,   "snmp-ups" },
};


DeviceConfigurations scanDevice(
    ScanProtocol protocol,
    std::string ipAddress,
    unsigned timeout,
    const std::vector<secw::DocumentPtr>& documents)
{
    return scanRangeDevices(protocol, ipAddress, ipAddress, timeout, documents);
}

DeviceConfigurations scanRangeDevices(
    ScanProtocol protocol,
    std::string ipAddressStart,
    std::string ipAddressEnd,
    unsigned timeout,
    const std::vector<secw::DocumentPtr>& documents)
{
    std::string stdout, stderr;
    MlmSubprocess::Argv args {
        "nut-scanner",
        "--quiet",
        "--disp_parsable",
        s_scanProtocols.at(protocol),
        "--start_ip", ipAddressStart
    };

    if (ipAddressStart != ipAddressEnd) {
        args.emplace_back("--end_ip");
        args.emplace_back(ipAddressEnd);
    }

    std::vector<KeyValues> documentParameters;
    std::transform(
        documents.begin(),
        documents.end(),
        std::back_inserter(documentParameters),
        std::bind(convertSecwDocumentToKeyValues, std::placeholders::_1, s_driverProtocols.at(protocol))
    );

    for (auto& documentParameter : documentParameters) {
        // Remove SnmpV3's "snmp_version" from list of arguments.
        auto it = documentParameter.find("snmp_version");
        if (it != documentParameter.end()) {
            documentParameter.erase(it);
        }

        for (const auto& parameter : documentParameter) {
            args.emplace_back("--" + parameter.first);
            args.emplace_back(parameter.second);
        }
    }

    (void)priv::runCommand(args, stdout, stderr, timeout);

    return parseScannerOutput(stdout);
}

}
}
