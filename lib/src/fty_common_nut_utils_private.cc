/*  =========================================================================
    fty_common_nut_utils_private - class description

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

#include "fty_common_nut_utils_private.h"
#include <fty_log.h>

namespace fty::nut::priv {

int runCommand(
    const std::string& cmd, const Process::Arguments& args, std::string& stdout, std::string& stderr, int timeout)
{
    std::stringstream fullCommand;
    for (const auto& i : args) {
        fullCommand << i << " ";
    }
    std::string fullCommandStr = fullCommand.str();
    log_info("Running command %s(with %d seconds timeout)...", fullCommandStr.c_str(), timeout);

    Process proc(cmd, args, Capture::Out | Capture::Err);
    if (auto pid = proc.run(); !pid) {
        log_error(pid.error().c_str());
        return -1;
    }

    auto ret = proc.wait(timeout);
    if (!ret) {
        log_error(ret.error().c_str());
        return -1;
    }

    stdout = proc.readAllStandardOutput();
    stderr = proc.readAllStandardError();

    if (!stdout.empty()) {
        log_trace("Standard output:\n%s", stdout.c_str());
    }
    if (!stderr.empty()) {
        log_trace("Standard error:\n%s", stderr.c_str());
    }

    if (*ret == 0) {
        log_info("Execution of command %ssucceeded.", fullCommandStr.c_str());
    } else {
        log_error("Execution of command %sfailed with code %d.", fullCommandStr.c_str(), *ret);
    }

    return *ret;
}

} // namespace fty::nut::priv
