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

/*
@header
    fty_common_nut_parse -
@discuss
@end
*/

#ifndef FTY_COMMON_NUT_PARSE_H_INCLUDED
#define FTY_COMMON_NUT_PARSE_H_INCLUDED

#include "fty_common_nut_library.h"

namespace fty {
namespace nut {

DeviceConfigurations parseConfigurationFile(const std::string& in);
DeviceConfigurations parseScannerOutput(const std::string& in);
KeyValues parseDumpOutput(const std::string& in);

}
}

std::ostream& operator<<(std::ostream &out, const fty::nut::DeviceConfiguration &cfg);

//  Self test of this class

void fty_common_nut_parse_test(bool verbose);

#endif
