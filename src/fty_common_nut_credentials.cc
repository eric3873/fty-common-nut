/*  =========================================================================
    fty_common_nut_credentials - class description

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
    fty_common_nut_credentials -
@discuss
@end
*/

#include "fty_common_nut_classes.h"

namespace nutcommon {

static std::string s_getUniqueClientId()
{
    ZuuidGuard uuid(zuuid_new());
    return std::string("fty-common-nut-") + zuuid_str(uuid.get());
}

using DocumentFilter = std::function<bool(const secw::Document*)>;

std::vector<CredentialsSNMPv3> getCredentialsSNMPv3(DocumentFilter pred)
{
    static const std::map<secw::Snmpv3AuthProtocol, std::string> authMapping = {
        { secw::MD5, "MD5" },
        { secw::SHA, "SHA" }
    } ;

    static const std::map<secw::Snmpv3PrivProtocol, std::string> privMapping = {
        { secw::DES, "DES" },
        { secw::AES, "AES" }
    } ;

    std::vector<CredentialsSNMPv3> creds;

    try {
        auto client = secw::ConsumerAccessor(s_getUniqueClientId(), 1000, MLM_ENDPOINT);
        auto secCreds = client.getListDocumentsWithPrivateData("default", "discovery_monitoring");

        for (const auto &i : secCreds) {
            if (!pred(i.get())) {
                continue;
            }

            auto cred = dynamic_cast<const secw::Snmpv3*>(i.get());
            if (cred) {
                std::string secName = cred->getSecurityName();
                std::string authPassword, authProtocol;
                std::string privPassword, privProtocol;

                if (cred->getSecurityLevel() != secw::NO_AUTH_NO_PRIV) {
                    authPassword = cred->getAuthPassword();
                    authProtocol = authMapping.at(cred->getAuthProtocol());

                    if (cred->getSecurityLevel() != secw::AUTH_NO_PRIV) {
                        privPassword = cred->getPrivPassword();
                        privProtocol = privMapping.at(cred->getPrivProtocol());
                    }
                }

                creds.emplace_back(secName, authPassword, authProtocol, privPassword, privProtocol);
            }
        }
        log_debug("Fetched %d SNMPv3 credentials from security wallet.", creds.size());
    }
    catch (std::exception &e) {
        log_warning("Failed to fetch SNMPv3 credentials from security wallet: %s", e.what());
    }

    return creds;
}

std::vector<CredentialsSNMPv1> getCredentialsSNMPv1(DocumentFilter pred)
{
    std::vector<CredentialsSNMPv1> creds;

    try {
        auto client = secw::ConsumerAccessor(s_getUniqueClientId(), 1000, MLM_ENDPOINT);
        auto secCreds = client.getListDocumentsWithPrivateData("default", "discovery_monitoring");

        for (const auto &i : secCreds) {
            if (!pred(i.get())) {
                continue;
            }

            auto cred = dynamic_cast<const secw::Snmpv1*>(i.get());
            if (cred) {
                creds.emplace_back(cred->getCommunityName());
            }
        }

        log_debug("Fetched %d SNMPv1 credentials from security wallet.", creds.size());
    }
    catch (std::exception &e) {
        log_warning("Failed to fetch SNMPv1 credentials from security wallet: %s", e.what());
    }

    return creds;
}

std::vector<CredentialsSNMPv3> getCredentialsSNMPv3() {
    return getCredentialsSNMPv3([](const secw::Document*) -> bool { return true; });
}

std::vector<CredentialsSNMPv1> getCredentialsSNMPv1() {
    return getCredentialsSNMPv1([](const secw::Document*) -> bool { return true; });
}

std::vector<CredentialsSNMPv3> getCredentialsSNMPv3(const std::set<std::string> &documentIds) {
    return getCredentialsSNMPv3([&documentIds](const secw::Document* document) -> bool {
        return documentIds.count(document->getId());
    });
}

std::vector<CredentialsSNMPv1> getCredentialsSNMPv1(const std::set<std::string> &documentIds) {
    return getCredentialsSNMPv1([&documentIds](const secw::Document* document) -> bool {
        return documentIds.count(document->getId());
    });
}

}
