#include "fty_common_nut.h"
#include <catch2/catch.hpp>
#include <sstream>

TEST_CASE("common nut parse test")
{
    // clang-format off
    static const fty::nut::DeviceConfigurations expectedDeviceConfigurationResult = {
        { { "name", "nutdev1"}, { "driver", "netxml-ups" }, { "port", "http://10.130.33.199" }, { "desc", "Mosaic 4M" } },
        { { "name", "nutdev2"}, { "driver", "netxml-ups" }, { "port", "http://10.130.33.194" }, { "desc", "Mosaic 4M 16M" } },
        { { "name", "nutdev3"}, { "driver", "snmp-ups" }, { "port", "10.130.33.252" }, { "desc", "ePDU MANAGED 38U-A IN L6-30P 24A 1P OUT 20xC13:4xC19" }, { "mibs", "eaton_epdu" }, { "community", "public" } },
        { { "name", "nutdev4"}, { "driver", "snmp-ups" }, { "port", "10.130.33.7" }, { "desc", "HP R1500 INTL UPS" }, { "mibs", "pw" }, { "community", "public" } },
        { { "name", "nutdev5"}, { "driver", "snmp-ups" }, { "port", "10.130.33.151" }, { "desc", "PX3-5493V" }, { "mibs", "raritan-px2" }, { "community", "public" } },
        { { "name", "nutdev6"}, { "driver", "snmp-ups" }, { "port", "10.130.32.117" }, { "desc", "Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M"}, { "mibs", "eaton_epdu" }, { "secLevel", "noAuthNoPriv" }, { "secName", "user1" } }
    };
    // clang-format on

    // fty::nut::parseConfigurationFile
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
        secName= user1)xxx";

        auto result = fty::nut::parseConfigurationFile(configurationFile);

        // Check structures are identical.
        CHECK(result.size() == expectedDeviceConfigurationResult.size());
        auto expectedIt = expectedDeviceConfigurationResult.begin();
        for (const auto& it : result) {
            CHECK(it.size() == expectedIt->size());
            auto expectedIt2 = expectedIt->begin();
            for (const auto& it2 : it) {
                CHECK(it2 == *expectedIt2);
                expectedIt2++;
            }
            expectedIt++;
        }
    }

    // fty::nut::parseScannerOutput
    {
        static const std::string scannerOutput =
            R"xxx(XML:driver="netxml-ups",port="http://10.130.33.199",desc="Mosaic 4M",name="nutdev1"
XML:driver="netxml-ups",port="http://10.130.33.194",desc="Mosaic 4M 16M",name="nutdev2"
SNMP:driver="snmp-ups",port="10.130.33.252",desc="ePDU MANAGED 38U-A IN L6-30P 24A 1P OUT 20xC13:4xC19",mibs="eaton_epdu",community="public",name="nutdev3"
SNMP:driver="snmp-ups",port="10.130.33.7",desc="HP R1500 INTL UPS",mibs="pw",community="public",name="nutdev4"
SNMP:driver="snmp-ups",port="10.130.33.151",desc="PX3-5493V",mibs="raritan-px2",community="public",name="nutdev5"
SNMP:driver="snmp-ups",port="10.130.32.117",desc="Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M",mibs="eaton_epdu",secLevel="noAuthNoPriv",secName="user1",name="nutdev6"
)xxx";

        auto result = fty::nut::parseScannerOutput(scannerOutput);

        // Check structures are identical.
        CHECK(result.size() == expectedDeviceConfigurationResult.size());
        auto expectedIt = expectedDeviceConfigurationResult.begin();
        for (const auto& it : result) {
            CHECK(it.size() == expectedIt->size());
            auto expectedIt2 = expectedIt->begin();
            for (const auto& it2 : it) {
                CHECK(it2 == *expectedIt2);
                expectedIt2++;
            }
            expectedIt++;
        }
    }

    // fty::nut::parseDumpOutput
    {
        // Launching the command by hand for the NetXML driver resulted in some extra junk at the beginning.
        static const std::string         dumpOutput     = R"xxx(Network UPS Tools - network XML UPS 0.42 (2.7.4.1)
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
        static const fty::nut::KeyValues expectedValues = {{"ambient.humidity.high", "90"},
            {"ambient.humidity.low", "5"}, {"ambient.temperature.high", "40"}, {"ambient.temperature.low", "5"},
            {"device.contact", "Computer Room Manager"}, {"device.location", "Computer Room"}, {"device.mfr", "EATON"},
            {"device.model", "HP R/T3000 HV INTL UPS"}, {"device.type", "ups"}, {"driver.name", "netxml-ups"},
            {"driver.parameter.port", "http://10.130.33.199"}, {"input.voltage", "244"}, {"outlet.1.status", "on"},
            {"outlet.1.switchable", "yes"}, {"output.voltage", "244"}, {"output.voltage.nominal", "230"},
            {"ups.beeper.status", "disabled"}, {"ups.mfr", "EATON"}, {"ups.model", "HP R/T3000 HV INTL UPS"},
            {"ups.model.aux", "UPS LI R"}, {"ups.type", "offline / line interactive"}};

        auto result = fty::nut::parseDumpOutput(dumpOutput);

        // Check structures are identical.
        CHECK(result.size() == expectedValues.size());
        auto expectedIt = expectedValues.begin();
        for (const auto& it : result) {
            CHECK(it == *expectedIt);
            expectedIt++;
        }
    }

    // operator<< for fty::nut::DeviceConfiguration
    {
        static const std::string outputReference = "[nutdev6]\n"
            "\tdesc = \"Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M\"\n"
            "\tdriver = \"snmp-ups\"\n"
            "\tmibs = \"eaton_epdu\"\n"
            "\tport = \"10.130.32.117\"\n"
            "\tsecLevel = \"noAuthNoPriv\"\n"
            "\tsecName = \"user1\"\n";

        static const fty::nut::DeviceConfiguration inputData = {{"name", "nutdev6"}, {"driver", "snmp-ups"},
            {"port", "10.130.32.117"}, {"desc", "Eaton ePDU MA 1P IN:C20 16A OUT:20xC13, 4xC19M"},
            {"mibs", "eaton_epdu"}, {"secLevel", "noAuthNoPriv"}, {"secName", "user1"}};

        std::stringstream ss;
        ss << inputData;
        std::string outputData = ss.str();
        CHECK(outputReference == outputData);
    }
}
