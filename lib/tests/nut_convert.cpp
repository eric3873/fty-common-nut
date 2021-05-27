#include <catch2/catch.hpp>
#include "fty_common_nut.h"

#define SELFTEST_RO "tests/selftest-ro"

TEST_CASE("common nut convert test")
{
    // clang-format off
    const static std::vector<std::pair<std::string, std::string>> invalidTestCases = {
        {SELFTEST_RO "/nosuchfile.conf",        "noSuchMapping"},
        {SELFTEST_RO "/mappingInvalid.conf",    "noSuchMapping"},
        {SELFTEST_RO "/mappingValid.conf",      "noSuchMapping"},
        {SELFTEST_RO "/mappingValid.conf",      "emptyMapping"},
        {SELFTEST_RO "/mappingValid.conf",      "badMapping"}
    };
    // clang-format on

    // Test invalid mapping cases.
    for (const auto& i : invalidTestCases) {
        bool caughtException = false;
        try {
            fty::nut::KeyValues mapping = fty::nut::loadMapping(i.first, i.second);
        } catch (...) {
            caughtException = true;
        }
        CHECK(caughtException);
    }

    // Test valid mapping cases.
    const auto physicsMapping   = fty::nut::loadMapping(SELFTEST_RO "/mappingValid.conf", "physicsMapping");
    const auto inventoryMapping = fty::nut::loadMapping(SELFTEST_RO "/mappingValid.conf", "inventoryMapping");
    CHECK(!physicsMapping.empty());
    CHECK(!inventoryMapping.empty());
}
