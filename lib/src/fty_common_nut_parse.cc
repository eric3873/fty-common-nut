/*  =========================================================================
    fty_common_nut_parse - class description

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

#include "fty_common_nut_parse.h"
#include <iostream>
#include <regex>

namespace fty::nut {

DeviceConfigurations parseConfigurationFile(const std::string& in)
{
    static const std::regex regexSection(R"xxx([[:blank:]]*\[([[:alnum:]_-]+)\][[:blank:]]*)xxx", std::regex::optimize);
    static const std::regex regexOptionQuoted(
        R"xxx([[:blank:]]*([[:alpha:]_-]+)[[:blank:]]*=[[:blank:]]*"([^"]+)"[[:blank:]]*)xxx", std::regex::optimize);
    static const std::regex regexOptionUnquoted(
        R"xxx([[:blank:]]*([[:alpha:]_-]+)[[:blank:]]*=[[:blank:]]*([^"].*))xxx", std::regex::optimize);
    std::smatch       matches;
    std::stringstream inStream(in);
    std::string       line;

    DeviceConfigurations devices;
    DeviceConfiguration  device;

    while (std::getline(inStream, line)) {
        if (std::regex_match(line, matches, regexSection)) {
            // Section matched, flush current device if applicable and start anew.
            if (!device.empty()) {
                devices.emplace_back(device);
            }
            device.clear();
            device.emplace("name", matches[1].str());
        } else if (std::regex_match(line, matches, regexOptionQuoted) ||
                   std::regex_match(line, matches, regexOptionUnquoted)) {
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
    std::stringstream       inStream(in);
    std::string             line;

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
    std::smatch             matches;
    std::stringstream       inStream(in);
    std::string             line;

    KeyValues entries;

    while (std::getline(inStream, line)) {
        if (std::regex_match(line, matches, regexEntry)) {
            entries.emplace(matches[1].str(), matches[2].str());
        }
    }

    return entries;
}

} // namespace fty::nut

std::ostream& operator<<(std::ostream& out, const fty::nut::DeviceConfiguration& cfg)
{
    std::string name = "<unknown>";
    if (cfg.count("name")) {
        name = cfg.at("name");
    }

    out << "[" << name << "]" << std::endl;
    for (const auto& i : cfg) {
        if (i.first == "name") {
            continue;
        }

        out << "\t" << i.first << " = \"" << i.second << "\"" << std::endl;
    }

    return out;
}
