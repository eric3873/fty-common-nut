/*  =========================================================================
    fty_common_nut_parse - class description

    Copyright (C) 2014 - 2018 Eaton

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
    fty_common_nut_parse -
@discuss
@end
*/

#include "fty_common_nut_classes.h"

#include <iostream>
#include <regex>

namespace nutcommon {

DeviceConfigurations parseConfigurationFile(const std::string& in)
{
    static const std::regex regexSection(R"xxx([[:blank:]]*\[([[:alnum:]_-]+)\][[:blank:]]*)xxx", std::regex::optimize);
    static const std::regex regexOptionQuoted(R"xxx([[:blank:]]*([[:alpha:]_-]+)[[:blank:]]*=[[:blank:]]*"([^"]+)"[[:blank:]]*)xxx", std::regex::optimize);
    static const std::regex regexOptionUnquoted(R"xxx([[:blank:]]*([[:alpha:]_-]+)[[:blank:]]*=[[:blank:]]*([^"].*))xxx", std::regex::optimize);
    std::smatch matches;
    std::stringstream inStream(in);
    std::string line;

    DeviceConfigurations devices;
    DeviceConfiguration device;

    while (std::getline(inStream, line)) {
        if (std::regex_match(line, matches, regexSection)) {
            // Section matched, flush current device if applicable and start anew.
            if (!device.empty()) {
                devices.emplace_back(device);
            }
            device.clear();
            device.emplace("name", matches[1].str());
        }
        else if (std::regex_match(line, matches, regexOptionQuoted) || std::regex_match(line, matches, regexOptionUnquoted)) {
            // Key-value pair matched, add it to the list.
            device.emplace(matches[1].str(), matches[2].str());
        }
    }

    // Flush current device if applicable.
    if (!device.empty()) {
        devices.emplace_back(device);
    }

    return devices;
}

DeviceConfigurations parseScannerOutput(const std::string& in)
{
    /**
     * This regex matches data in the form of (ignored:)name="value"(,) and thus matches
     * one key-value pair.
     */
    static const std::regex regexEntry(R"xxx((?:[[:alpha:]]+:)?([[:alpha:]_-]+)="([^"]*)",?)xxx", std::regex::optimize);
    std::smatch matches;
    std::stringstream inStream(in);
    std::string line;

    DeviceConfigurations devices;

    while (std::getline(inStream, line)) {
        DeviceConfiguration device;

        auto begin = std::sregex_iterator(line.begin(), line.end(), regexEntry);
        for (auto it = begin; it != std::sregex_iterator(); it++) {
            // Iterate over all key-value pairs matches.
            device.emplace((*it)[1].str(), (*it)[2].str());
        }

        devices.emplace_back(device);
    }

    return devices;
}

KeyValues parseDumpOutput(const std::string& in)
{
    static const std::regex regexEntry(R"xxx(([a-z0-9.]+): (.*))xxx", std::regex::optimize);
    std::smatch matches;
    std::stringstream inStream(in);
    std::string line;

    KeyValues entries;

    while (std::getline(inStream, line)) {
        if (std::regex_match(line, matches, regexEntry)) {
            entries.emplace(matches[1].str(), matches[2].str());
        }
    }

    return entries;
}

}

std::ostream& operator<<(std::ostream &out, const nutcommon::DeviceConfiguration &cfg)
{
    const auto& name = cfg.at("name");
    out << "[" << name << "]" << std::endl;
    for (const auto &i : cfg) {
        if (i.first == "name") {
            continue;
        }

        out << "\t" << i.first << " = \"" << i.second << "\"" << std::endl;
    }

    return out;
}

//  --------------------------------------------------------------------------
//  Self test of this class

void fty_common_nut_parse_test(bool verbose)
{
    std::cout << " * fty_common_nut_parse: ";

    static const nutcommon::DeviceConfigurations expectedDeviceConfigurationResult = {
        { { "name", "nutdev1"}, { "driver", "netxml-ups" }, { "port", "http://10.130.33.199" }, { "desc", "Mosaic 4M" } },
        { { "name", "nutdev2"}, { "driver", "netxml-ups" }, { "port", "http://10.130.33.194" }, { "desc", "Mosaic 4M 16M" } },
        { { "name", "nutdev3"}, { "driver", "snmp-ups" }, { "port", "10.130.33.252" }, { "desc", "ePDU MANAGED 38U-A IN L6-30P 24A 1P OUT 20xC13:4xC19" }, { "mibs", "eaton_epdu" }, { "community", "public" } },
        { { "name", "nutdev4"}, { "driver", "snmp-ups" }, { "port", "10.130.33.7" }, { "desc", "HP R1500 INTL UPS" }, { "mibs", "pw" }, { "community", "public" } },
        { { "name", "nutdev5"}, { "driver", "snmp-ups" }, { "port", "10.130.33.151" }, { "desc", "PX3-5493V" }, { "mibs", "raritan-px2" }, { "community", "public" } },
        { { "name", "nutdev6"}, { "driver", "snmp-ups" }, { "port", "10.130.32.117" }, { "desc", "Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M"}, { "mibs", "eaton_epdu" }, { "secLevel", "noAuthNoPriv" }, { "secName", "user1" } },
        { { "name", "nutdev7"}, { "driver", "nutdrv_modbus" }, { "port", "10.130.21.9" }, { "slave_id", "1" }, { "desc", "Eaton EMECMODB"}, { "mapping", "eaton_pxmeter" } }
    };

    // nutcommon::parseConfigurationFile
    {
        static const std::string configurationFile = R"xxx([nutdev1]
        driver = "netxml-ups"
        port = "http://10.130.33.199"
        desc = "Mosaic 4M"
[nutdev2]
        driver = "netxml-ups"
        port = "http://10.130.33.194"
        desc = "Mosaic 4M 16M"
[nutdev3]
        driver = "snmp-ups"
        port = "10.130.33.252"
        desc = "ePDU MANAGED 38U-A IN L6-30P 24A 1P OUT 20xC13:4xC19"
        mibs = "eaton_epdu"
        community = "public"
[nutdev4]
        driver = "snmp-ups"
        port = "10.130.33.7"
        desc = "HP R1500 INTL UPS"
        mibs = "pw"
        community = "public"
[nutdev5]
        driver = "snmp-ups"
        port = "10.130.33.151"
        desc = "PX3-5493V"
        mibs = "raritan-px2"
        community = "public"
[nutdev6]
        driver="snmp-ups"
        port=10.130.32.117
        desc = Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M
        mibs =eaton_epdu
        secLevel ="noAuthNoPriv"
        secName= user1
[nutdev7]
    driver="nutdrv_modbus"
    port=10.130.21.9
    slave_id="1"
    desc="Eaton EMECMODB"
    mapping="eaton_pxmeter")xxx";

        auto result = nutcommon::parseConfigurationFile(configurationFile);

        // Check structures are identical.
        assert(result.size() == expectedDeviceConfigurationResult.size());
        auto expectedIt = expectedDeviceConfigurationResult.begin();
        for (const auto &it : result) {
            assert(it.size() == expectedIt->size());
            auto expectedIt2 = expectedIt->begin();
            for (const auto &it2 : it) {
                assert(it2 == *expectedIt2);
                expectedIt2++;
            }
            expectedIt++;
        }
    }

    // nutcommon::parseScannerOutput
    {
        static const std::string scannerOutput = R"xxx(XML:driver="netxml-ups",port="http://10.130.33.199",desc="Mosaic 4M",name="nutdev1"
XML:driver="netxml-ups",port="http://10.130.33.194",desc="Mosaic 4M 16M",name="nutdev2"
SNMP:driver="snmp-ups",port="10.130.33.252",desc="ePDU MANAGED 38U-A IN L6-30P 24A 1P OUT 20xC13:4xC19",mibs="eaton_epdu",community="public",name="nutdev3"
SNMP:driver="snmp-ups",port="10.130.33.7",desc="HP R1500 INTL UPS",mibs="pw",community="public",name="nutdev4"
SNMP:driver="snmp-ups",port="10.130.33.151",desc="PX3-5493V",mibs="raritan-px2",community="public",name="nutdev5"
SNMP:driver="snmp-ups",port="10.130.32.117",desc="Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M",mibs="eaton_epdu",secLevel="noAuthNoPriv",secName="user1",name="nutdev6"
MODBUS:driver="nutdrv_modbus",port="10.130.21.9",slave_id="1",desc="Eaton EMECMODB",mapping="eaton_pxmeter"
)xxx";

        auto result = nutcommon::parseScannerOutput(scannerOutput);

        // Check structures are identical.
        assert(result.size() == expectedDeviceConfigurationResult.size());
        auto expectedIt = expectedDeviceConfigurationResult.begin();
        for (const auto &it : result) {
            assert(it.size() == expectedIt->size());
            auto expectedIt2 = expectedIt->begin();
            for (const auto &it2 : it) {
                assert(it2 == *expectedIt2);
                expectedIt2++;
            }
            expectedIt++;
        }
    }

    // nutcommon::parseDumpOutput
    {
        // Launching the command by hand for the NetXML driver resulted in some extra junk at the beginning.
        static const std::string dumpOutput = R"xxx(Network UPS Tools - network XML UPS 0.42 (2.7.4.1)
Warning: This is an experimental driver.
Some features may not function correctly.

ambient.humidity.high: 90
ambient.humidity.low: 5
ambient.temperature.high: 40
ambient.temperature.low: 5
device.contact: Computer Room Manager
device.location: Computer Room
device.mfr: EATON
device.model: HP R/T3000 HV INTL UPS
device.type: ups
driver.name: netxml-ups
driver.parameter.port: http://10.130.33.199
input.voltage: 244
outlet.1.status: on
outlet.1.switchable: yes
output.voltage: 244
output.voltage.nominal: 230
ups.beeper.status: disabled
ups.mfr: EATON
ups.model: HP R/T3000 HV INTL UPS
ups.model.aux: UPS LI R
ups.type: offline / line interactive
)xxx";
        static const nutcommon::KeyValues expectedValues = {
            { "ambient.humidity.high", "90" },
            { "ambient.humidity.low", "5" },
            { "ambient.temperature.high", "40" },
            { "ambient.temperature.low", "5" },
            { "device.contact", "Computer Room Manager" },
            { "device.location", "Computer Room" },
            { "device.mfr", "EATON" },
            { "device.model", "HP R/T3000 HV INTL UPS" },
            { "device.type", "ups" },
            { "driver.name", "netxml-ups" },
            { "driver.parameter.port", "http://10.130.33.199" },
            { "input.voltage", "244" },
            { "outlet.1.status", "on" },
            { "outlet.1.switchable", "yes" },
            { "output.voltage", "244" },
            { "output.voltage.nominal", "230" },
            { "ups.beeper.status", "disabled" },
            { "ups.mfr", "EATON" },
            { "ups.model", "HP R/T3000 HV INTL UPS" },
            { "ups.model.aux", "UPS LI R" },
            { "ups.type", "offline / line interactive" }
        };

        auto result = nutcommon::parseDumpOutput(dumpOutput);

        // Check structures are identical.
        assert(result.size() == expectedValues.size());
        auto expectedIt = expectedValues.begin();
        for (const auto &it : result) {
            assert(it == *expectedIt);
            expectedIt++;
        }
    }

    // operator<< for nutcommon::DeviceConfiguration
    {
        static const std::string outputReference = R"xxx([nutdev6]
	desc = "Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M"
	driver = "snmp-ups"
	mibs = "eaton_epdu"
	port = "10.130.32.117"
	secLevel = "noAuthNoPriv"
	secName = "user1"
)xxx";
        static const nutcommon::DeviceConfiguration inputData = {
            { "name", "nutdev6" },
            { "driver", "snmp-ups" },
            { "port", "10.130.32.117" },
            { "desc", "Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M" },
            { "mibs", "eaton_epdu" },
            { "secLevel", "noAuthNoPriv" },
            { "secName", "user1" }
        } ;

        std::stringstream ss;
        ss << inputData;
        std::string outputData = ss.str();
        assert(outputReference == outputData);
    }

    std::cout << "OK" << std::endl;
}