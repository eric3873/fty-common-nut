/*  =========================================================================
    fty_common_nut_credentials - class description

    Copyright (C)
        2014 - 2018 Eaton
        2019        Arnaud Quette <arnaud.quette@free.fr>

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
    fty_common_nut_credentials -
@discuss
@end
*/

#ifndef FTY_COMMON_NUT_CREDENTIALS_H_INCLUDED
#define FTY_COMMON_NUT_CREDENTIALS_H_INCLUDED

namespace nutcommon {

struct CredentialsSNMPv1
{
    CredentialsSNMPv1(const std::string& comm) : community(comm) {}

    std::string community;
};

struct CredentialsSNMPv3
{
    CredentialsSNMPv3(const std::string& name,
        const std::string& authPass, const std::string& authProto,
        const std::string& privPass, const std::string& privProto
    ) : secName(name),
        authPassword(authPass), authProtocol(authProto),
        privPassword(privPass), privProtocol(privProto) {}

    std::string secName;
    std::string authPassword;
    std::string authProtocol;
    std::string privPassword;
    std::string privProtocol;
};

// Note: for now, this CredentialsModbus is done to please compilers
// In the future, it may be used for Modbus Security
struct CredentialsModbus
{
    CredentialsModbus() : {}
};

std::vector<CredentialsSNMPv1> getCredentialsSNMPv1();
std::vector<CredentialsSNMPv3> getCredentialsSNMPv3();
std::vector<CredentialsModbus> getCredentialsModbus();

}

#endif
