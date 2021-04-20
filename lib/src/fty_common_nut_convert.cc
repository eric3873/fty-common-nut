/*  =========================================================================
    fty_common_nut_convert - class description

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
    fty_common_nut_convert -
@discuss
@end
*/

#include "fty_common_nut_classes.h"

#include <cxxtools/jsondeserializer.h>
#include <fstream>
#include <iostream>
#include <regex>

namespace fty {
namespace nut {

static std::string performSingleMapping(const KeyValues &mapping, const std::string &key, int index)
{
    // Need to capture #n:
    // device.n.xxx - daisychain device
    // device.n.ambient.xxx  -> emp01 with daisychain (only one sensor on a device #n)
    // ambient.n.xxx  -> emp02 with no daisychain
    // device.1.ambient.n.xxx -> emp02 with daisychain (all sensor are on master #1)
    const static std::regex prefixRegex(R"xxx(.*((?:device(?!\.\d*\.ambient\.\d*\.))|ambient)\.(\d*)\.(.+))xxx", std::regex::optimize);
    std::smatch matches;

    std::string transformedKey = key;

    log_trace("%s: Looking for key %s (index %i)", __func__, key.c_str(), index);

    // Daisy-chained or indexed sensor special case, need to fold it back into conventional case.
    if (index > 0 && std::regex_match(key, matches, prefixRegex)) {
        log_trace("performSingleMapping: match1 = %s, match2 = %s, match3 = %s", matches.str(1).c_str(), matches.str(2).c_str(), matches.str(3).c_str());
        if (matches.str(2) == std::to_string(index)) {
            log_trace("%s: key %s (index %i) 1-> found and test %s", __func__, key.c_str(), index, matches.str(1) + "." + matches.str(3));
            // We have a "{device,ambient}.<id>.<property>" property, map it to either device.<property> or ambient.<property> or <property> (for device only)
            if (mapping.find(matches.str(1) + "." + matches.str(3)) != mapping.end()) {
                transformedKey = matches.str(1) + "." + matches.str(3);
                //log_trace("%s: key %s (index %i) -> %s", __func__, key.c_str(), index, transformedKey.c_str());
            }
            else {
                if (matches.str(1) != "ambient") {
                    transformedKey = matches.str(3);
                    //log_trace("%s: key %s (index %i) -> %s", __func__, key.c_str(), index, transformedKey.c_str());
                }
            }
        }
        else {
            // Not the daisy-chained or sensor index we're looking for.
            transformedKey = "";
        }
    }

    auto mappedKey = mapping.find(transformedKey);
    return mappedKey == mapping.cend() ? "" : mappedKey->second;
}

KeyValues performMapping(const KeyValues &mapping, const KeyValues &values, int index)
{
    const static std::regex overrideRegex(R"xxx((device|ambient)\.([^[:digit:]].*))xxx", std::regex::optimize);
    const std::string strIndex = std::to_string(index);

    KeyValues mappedValues;

    for (auto value : values) {
        const std::string mappedKey = performSingleMapping(mapping, value.first, index);

        log_trace("%s: got mappedKey '%s'", __func__, mappedKey.c_str());

        // Let daisy-chained device data override host device data (device.<id>.<property> => device.<property> or <property>).
        std::smatch matches;
        if (index > 0 && std::regex_match(value.first, matches, overrideRegex)) {
            log_trace("performMapping: match1 = %s, match2 = %s, match3 = %s", matches.str(1).c_str(), matches.str(2).c_str(), matches.str(3).c_str());
            if (values.count(matches.str(1) + "." + strIndex + "." + matches.str(2))) {
                log_trace("Ignoring overriden property '%s' during mapping (daisy-chain override).", value.first.c_str());
                continue;
            }
        }

        // Let input.L1.current override input.current (3-phase UPS).
        if (value.first == "input.current" && values.count("input.L1.current")) {
            log_trace("Ignoring overriden property '%s' during mapping (3-phase UPS input current override).", value.first.c_str());
            continue;
        }

        if (!mappedKey.empty()) {
            log_trace("Mapped property '%s' to '%s' (value='%s').", value.first.c_str(), mappedKey.c_str(), value.second.c_str());
            mappedValues.emplace(mappedKey, value.second);
        }
    }

    log_trace("Mapped %d/%d properties.", mappedValues.size(), values.size());
    return mappedValues;
}

KeyValues loadMapping(const std::string &file, const std::string &type)
{
    KeyValues result;
    std::stringstream err;

    std::ifstream input(file);
    if (!input) {
        err << "Error opening file '" << file << "'";
        throw std::runtime_error(err.str());
    }

    // Parse JSON.
    cxxtools::SerializationInfo si;
    cxxtools::JsonDeserializer deserializer(input);
    try {
        deserializer.deserialize();
    }
    catch (std::exception &e) {
        err << "Couldn't parse mapping file '" << file << "': " << e.what() << ".";
        throw std::runtime_error(err.str());
    }

    const cxxtools::SerializationInfo *mapping = deserializer.si()->findMember(type);
    if (mapping == nullptr) {
        err << "No mapping type '" << type << "' in mapping file '" << file << "'.";
        throw std::runtime_error(err.str());
    }
    if (mapping->category () != cxxtools::SerializationInfo::Category::Object) {
        err << "Mapping type '" << type << "' in mapping file '" << file << "' is not a JSON object.";
        throw std::runtime_error(err.str());
    }

    // Convert all mappings.
    for (const auto& i : *mapping) {
        std::string name = i.name();

        try {
            if (i.category () != cxxtools::SerializationInfo::Category::Value) {
                throw std::runtime_error("Not a JSON atomic value");
            }

            std::string value;
            i.getValue(value);

            auto x = name.find("#");
            auto y = value.find("#");
            // normal mapping means no index in the source AND dest
            if (x == std::string::npos && y == std::string::npos) {
            // if (x == std::string::npos || y == std::string::npos) {
                // Normal mapping, insert it.
                result.emplace(std::make_pair(name, value));
            }
            else {
                // either (src AND dest) have index or just src (as with ambient)
                // Template mapping, instanciate it.
                for (int i = 1; i < 99; i++) {
                    std::string instanceName = name;
                    std::string instanceValue = value;
                    if (x != std::string::npos)
                        instanceName.replace(x, 1, std::to_string(i));
                    if (y != std::string::npos)
                        instanceValue.replace(y, 1, std::to_string(i));
                    result.emplace(std::make_pair(instanceName, instanceValue));
                }
            }
        }
        catch (std::exception &e) {
            log_warning("Can't deserialize key '%s.%s' in mapping file '%s' into string: %s.", type.c_str(), name.c_str(), file.c_str(), e.what());
        }
    }

    if (result.empty()) {
        err << "Mapping type '" << type << "' in mapping file '" << file << "' is empty.";
        throw std::runtime_error(err.str());
    }
    return result;
}

}
}

//  --------------------------------------------------------------------------
//  Self test of this class

void fty_common_nut_convert_test(bool verbose)
{
    #define SELFTEST_RO "selftest-ro"

    std::cout << " * fty_common_nut_convert: ";

    const static std::vector<std::pair<std::string, std::string>> invalidTestCases = {
        { SELFTEST_RO "/nosuchfile.conf", "noSuchMapping" },
        { SELFTEST_RO "/mappingInvalid.conf", "noSuchMapping" },
        { SELFTEST_RO "/mappingValid.conf", "noSuchMapping" },
        { SELFTEST_RO "/mappingValid.conf", "emptyMapping" },
        { SELFTEST_RO "/mappingValid.conf", "badMapping" }
    } ;

    // Test invalid mapping cases.
    for (const auto& i : invalidTestCases) {
        bool caughtException = false;
        try {
            fty::nut::KeyValues mapping = fty::nut::loadMapping(i.first, i.second);
        }
        catch (...) {
            caughtException = true;
        }
        assert(caughtException);
    }

    // Test valid mapping cases.
    const auto physicsMapping = fty::nut::loadMapping(SELFTEST_RO "/mappingValid.conf", "physicsMapping");
    const auto inventoryMapping = fty::nut::loadMapping(SELFTEST_RO "/mappingValid.conf", "inventoryMapping");
    assert(!physicsMapping.empty());
    assert(!inventoryMapping.empty());

    std::cout << "OK" << std::endl;
}
