cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

##############################################################################################################

project(fty_common_nut VERSION 1.0.0)

set(PROJECT_NAME_HYPHEN fty-common-nut)

########################################################################################################################

find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR})

########################################################################################################################

# lib & tests
add_subdirectory(lib)

## install
# https://cmake.org/cmake/help/v3.0/module/GNUInstallDirs.html

# mapping.conf -> share
install(FILES ${PROJECT_SOURCE_DIR}/resources/mapping.conf DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/${PROJECT_NAME_HYPHEN})
