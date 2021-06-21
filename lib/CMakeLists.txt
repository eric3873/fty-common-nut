cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

########################################################################################################################

etn_target(shared ${PROJECT_NAME}
    SOURCES
        src/fty_common_nut_convert.cc
        src/fty_common_nut_credentials.cc
        src/fty_common_nut_dump.cc
        src/fty_common_nut_parse.cc
        src/fty_common_nut_scan.cc
        src/fty_common_nut_utils_private.cc
        src/fty_common_nut_utils_private.h
    PUBLIC_INCLUDE_DIR
        public_include
    PUBLIC_HEADERS
        fty_common_nut_convert.h
        fty_common_nut_credentials.h
        fty_common_nut_dump.h
        fty_common_nut.h
        fty_common_nut_parse.h
        fty_common_nut_scan.h
    USES
        cxxtools
        fty_common
        fty-utils
        fty_common_logging
        fty_security_wallet
)

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

########################################################################################################################

etn_test_target(${PROJECT_NAME}
    CONFIGS
        tests/selftest-ro/mappingInvalid.conf
        tests/selftest-ro/mappingValid.conf
    SOURCES
        tests/main.cc
        tests/nut_convert.cpp
        tests/nut_parse.cpp
    INCLUDE_DIR
        include_public
    PREPROCESSOR
        -DCATCH_CONFIG_FAST_COMPILE
    USES
        stdc++fs
    SUBDIR
        tests
)

########################################################################################################################
