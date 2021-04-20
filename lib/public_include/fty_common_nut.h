/*  =========================================================================
    fty-common-nut - Provides common NUT tools for agents

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

#ifndef FTY_COMMON_NUT_H_H_INCLUDED
#define FTY_COMMON_NUT_H_H_INCLUDED

//  External dependencies
#include <cxxtools/allocator.h>
#include <tntdb.h>
#include <fty_log.h>
#include <fty_common_mlm.h>
#include <fty_security_wallet.h>

//  FTY_COMMON_NUT version macros for compile-time API detection
#define FTY_COMMON_NUT_VERSION_MAJOR 1
#define FTY_COMMON_NUT_VERSION_MINOR 0
#define FTY_COMMON_NUT_VERSION_PATCH 0

#define FTY_COMMON_NUT_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))

#define FTY_COMMON_NUT_VERSION \
    FTY_COMMON_NUT_MAKE_VERSION(FTY_COMMON_NUT_VERSION_MAJOR, FTY_COMMON_NUT_VERSION_MINOR, FTY_COMMON_NUT_VERSION_PATCH)

//  Public classes
#include "fty_common_nut_credentials.h"
#include "fty_common_nut_convert.h"
#include "fty_common_nut_dump.h"
#include "fty_common_nut_parse.h"
#include "fty_common_nut_scan.h"

#endif
