/*  =========================================================================
    fty_common_nut_credentials - class description

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

#include "fty_common_nut_dump.h"
#include "fty_common_nut_credentials.h"
#include "fty_common_nut_parse.h"
#include "fty_common_nut_utils_private.h"

namespace fty::nut {

KeyValues dumpDevice(const std::string& driver, const std::string& port, unsigned loopNb, unsigned loopIterTime,
    const std::vector<secw::DocumentPtr>& documents, const KeyValues& extra)
{
    // Build list of parameters.
    KeyValues data = extra;
    for (const auto& document : documents) {
        KeyValues documentKeyValues = convertSecwDocumentToKeyValues(document, driver);
        data.insert(documentKeyValues.begin(), documentKeyValues.end());
    }
    data.emplace("port", port);

    // Build command invocation.
    Process::Arguments args = {"-d", std::to_string(loopNb), "-u", "root", "-s",
        std::string("dumpdata-") + std::to_string(rand() % 100000 + 1)};

    for (const auto& it : data) {
        args.emplace_back("-x");
        args.emplace_back(it.first + "=" + it.second);
    }

    // Invoke command.
    std::string stdout, stderr;
    priv::runCommand("/lib/nut/" + driver, args, stdout, stderr, int(loopNb * loopIterTime));
    return parseDumpOutput(stdout);
}

} // namespace fty::nut
