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

#include "fty_common_nut_credentials.h"

namespace fty::nut {

static const std::map<secw::Snmpv3SecurityLevel, std::string> s_secMapping{
    {secw::NO_AUTH_NO_PRIV, "noAuthNoPriv"},
    {secw::AUTH_NO_PRIV, "authNoPriv"},
    {secw::AUTH_PRIV, "authPriv"},
};

static const std::map<secw::Snmpv3AuthProtocol, std::string> s_authMapping{
    {secw::MD5, "MD5"},
    {secw::SHA, "SHA"},
};

static const std::map<secw::Snmpv3PrivProtocol, std::string> s_privMapping{
    {secw::DES, "DES"},
    {secw::AES, "AES"},
};

KeyValues convertSecwDocumentToKeyValues(const secw::DocumentPtr& doc, const std::string& driver)
{
    if (driver.find_first_of("snmp-ups") == 0) {
        secw::Snmpv1Ptr snmpv1 = secw::Snmpv1::tryToCast(doc);
        secw::Snmpv3Ptr snmpv3 = secw::Snmpv3::tryToCast(doc);

        if (snmpv1) {
            return {{"community", snmpv1->getCommunityName()}};
        } else if (snmpv3) {
            KeyValues output{
                {"snmp_version", "v3"},
                {"secName", snmpv3->getSecurityName()},
                {"secLevel", s_secMapping.at(snmpv3->getSecurityLevel())},
            };

            if (snmpv3->getSecurityLevel() != secw::NO_AUTH_NO_PRIV) {
                output.emplace("authPassword", snmpv3->getAuthPassword());
                output.emplace("authProtocol", s_authMapping.at(snmpv3->getAuthProtocol()));

                if (snmpv3->getSecurityLevel() != secw::AUTH_NO_PRIV) {
                    output.emplace("privPassword", snmpv3->getPrivPassword());
                    output.emplace("privProtocol", s_privMapping.at(snmpv3->getPrivProtocol()));
                }
            }

            return output;
        } else {
            throw std::runtime_error(
                (std::string("Bad security wallet document type ") + doc->getType() + " for driver snmp-ups.").c_str());
        }
    } else if (driver == "etn-nut-powerconnect") {
        secw::UserAndPasswordPtr creds = secw::UserAndPassword::tryToCast(doc);

        if (creds) {
            return {{"username", creds->getUsername()}, {"password", creds->getPassword()}};
        } else {
            throw std::runtime_error((std::string("Bad security wallet document type ") + doc->getType() +
                                      " for driver etn-nut-powerconnect.")
                                         .c_str());
        }
    } else {
        throw std::runtime_error(
            (std::string("Unknown driver ") + driver + " for security wallet document conversion.").c_str());
    }
}

} // namespace fty::nut
