# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfIO )

# External dependencies:
find_package( Boost )
find_package( Oracle )     # Oracle client libraries for DB access
find_package( cx_Oracle )  # Oracle python client
find_package( nlohmann_json )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( TrigConfIO
  TrigConfIO/*.h src/*.cxx
  PUBLIC_HEADERS TrigConfIO
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES TrigConfData TrigConfBase
  PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} nlohmann_json::nlohmann_json -lstdc++fs
  )

atlas_add_executable( TestTriggerMenuAccess utils/TestTriggerMenuAccess.cxx
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfData TrigConfIO
  )

atlas_add_executable( TriggerMenuRW utils/TriggerMenuRW.cxx 
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfIO
  )

# Install files from the package.
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# => disabling this test for now until I find a better way to store the test menus
# Test(s) in the package.
# atlas_add_test( ReadTriggerConfig
#   SOURCES test/read_config_info.cxx
#   LINK_LIBRARIES TrigConfData TrigConfIO
#   ENVIRONMENT "TESTFILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/test/data"
#   POST_EXEC_SCRIPT nopost.sh
#   )

