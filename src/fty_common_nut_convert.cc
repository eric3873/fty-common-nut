/*  =========================================================================
    fty_common_nut_convert - class description

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
    fty_common_nut_convert -
@discuss
@end
*/

#include "fty_common_nut_classes.h"

#include <cxxtools/jsondeserializer.h>
#include <fstream>
#include <iostream>
#include <regex>

namespace nutcommon {

std::string extractDaisyChainedKey(const std::string &key, int id)
{
    const static std::regex prefixRegex(R"xxx(device\.([[:digit:]]+)\.(.+))xxx", std::regex::optimize);
    std::smatch matches;

    if (id > 0 && std::regex_match(key, matches, prefixRegex)) {
        if (matches[1] == std::to_string(id)) {
            // Remove the "device.<id>." prefix.
            return matches[2];
        }
        else {
            // Not the prefix we're looking for.
            return "";
        }
    }

    return key;
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
            if (x == std::string::npos || y == std::string::npos) {
                // Normal mapping, insert it.
                result.emplace(std::make_pair(name, value));
            }
            else {
                // Template mapping, instanciate it.
                for (int i = 1; i < 99; i++) {
                    std::string instanceName = name;
                    std::string instanceValue = value;
                    instanceName.replace(x, 1, std::to_string(i));
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

//  --------------------------------------------------------------------------
//  Self test of this class

void fty_common_nut_convert_test(bool verbose)
{
    std::cout << " * fty_common_nut_convert: ";

    const static std::vector<std::pair<std::string, std::string>> invalidTestCases = {
        { "src/selftest-ro/nosuchfile.conf", "noSuchMapping" },
        { "src/selftest-ro/mappingInvalid.conf", "noSuchMapping" },
        { "src/selftest-ro/mappingValid.conf", "noSuchMapping" },
        { "src/selftest-ro/mappingValid.conf", "emptyMapping" },
        { "src/selftest-ro/mappingValid.conf", "badMapping" }
    } ;

    // Test invalid mapping cases.
    for (const auto& i : invalidTestCases) {
        bool caughtException = false;
        try {
            nutcommon::KeyValues mapping = nutcommon::loadMapping(i.first, i.second);
        }
        catch (...) {
            caughtException = true;
        }
        assert(caughtException);
    }

    // Test valid mapping cases.
    const auto physicsMapping = nutcommon::loadMapping("src/selftest-ro/mappingValid.conf", "physicsMapping");
    const auto inventoryMapping = nutcommon::loadMapping("src/selftest-ro/mappingValid.conf", "inventoryMapping");
    assert(!physicsMapping.empty());
    assert(!inventoryMapping.empty());

    // Test daisy-chaining key extraction.
    {
        const static std::vector<std::string> in = {
            "device.count",
            "device.mfr",
            "device.1.key",
            "device.1.key1",
            "device.2.key",
            "device.2.key2"
        } ;

        const static std::vector<std::string> expected1 = {
            "device.count",
            "device.mfr",
            "key",
            "key1",
            "",
            ""
        } ;

        const static std::vector<std::string> expected2 = {
            "device.count",
            "device.mfr",
            "",
            "",
            "key",
            "key2"
        } ;

        const static std::vector<std::string> expected3 = {
            "device.count",
            "device.mfr",
            "",
            "",
            "",
            ""
        } ;

        assert(in.size() == expected1.size());
        assert(in.size() == expected2.size());
        assert(in.size() == expected3.size());

        auto itIn = in.cbegin();
        auto itExpected1 = expected1.cbegin();
        auto itExpected2 = expected2.cbegin();
        auto itExpected3 = expected3.cbegin();
        while (itIn != in.cend()) {
            assert(nutcommon::extractDaisyChainedKey(*itIn, 0) == *itIn);
            assert(nutcommon::extractDaisyChainedKey(*itIn, 1) == *itExpected1++);
            assert(nutcommon::extractDaisyChainedKey(*itIn, 2) == *itExpected2++);
            assert(nutcommon::extractDaisyChainedKey(*itIn, 3) == *itExpected3++);
            itIn++;
        }
    }

    std::cout << "OK" << std::endl;
}