# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfStorage )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system regex )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigConfStorage
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfStorage
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} TrigConfBase TrigConfHLTData TrigConfL1Data
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} TrigConfJobOptData L1TopoConfig )

atlas_add_executable( TrigConf2COOLApp
                      src/test/2COOLApp.cxx
                      LINK_LIBRARIES TrigConfStorage )

atlas_add_executable( TrigConfConsistencyChecker
                      src/test/ConsistencyChecker.cxx
                      LINK_LIBRARIES TrigConfStorage )

atlas_add_executable( TrigConfReadWrite
                      src/test/ReadWrite.cxx
                      LINK_LIBRARIES L1TopoConfig TrigConfJobOptData TrigConfStorage )

atlas_add_executable( TrigConfCoolFix
                      src/test/CoolFix.cxx
                      LINK_LIBRARIES TrigConfStorage )

atlas_add_executable( TrigConfTestTriggerDBCoolMix
                      src/test/TestTriggerDBCoolMix.cxx
                      LINK_LIBRARIES TrigConfStorage )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
