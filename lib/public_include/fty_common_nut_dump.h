/*  =========================================================================
    fty_common_nut_dump - class description

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
    fty_common_nut_dump -
@discuss
@end
*/

#ifndef FTY_COMMON_NUT_DUMP_H_INCLUDED
#define FTY_COMMON_NUT_DUMP_H_INCLUDED

namespace fty {
namespace nut {

/**
 * \brief Helper method to dump NUT data from a device.
 * \param driver Driver to use.
 * \param port Device to scan.
 * \param loopNb Number of acquisition loops to perform.
 * \param loopIterTime Max time per acquisition loop.
 * \param documents Security documents to use.
 * \param extra Extra parameters to pass to driver.
 * \return Map of key/value data returned by driver.
 */
KeyValues dumpDevice(
    const std::string& driver,
    const std::string& port,
    unsigned loopNb,
    unsigned loopIterTime,
    const std::vector<secw::DocumentPtr>& documents = {},
    const KeyValues& extra = {}
);

}
}

#endif
