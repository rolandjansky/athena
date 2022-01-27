# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfStorage )

# External dependencies:
find_package( Boost COMPONENTS regex )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_library( TrigConfStorage
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfStorage
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} TrigConfBase TrigConfL1Data
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} TrigConfJobOptData TrigConfHLTData L1TopoConfig )

if( NOT XAOD_STANDALONE )
   atlas_add_executable( TrigConfReadWrite
                         src/apps/ReadWrite.cxx src/apps/Run2toRun3ConvertersL1.cxx src/apps/Run2toRun3ConvertersHLT.cxx src/apps/TrigConfCoolWriter.cxx
                         INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                         LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils L1TopoConfig TrigCompositeUtilsLib TrigConfBase TrigConfData TrigConfHLTData TrigConfIO TrigConfJobOptData TrigConfL1Data TrigConfStorage nlohmann_json::nlohmann_json )

   atlas_add_test( test_Run2Run3Conversion
                   SCRIPT TrigConfReadWrite -i TRIGGERDB 2218,4229,2967,1373 -o r3json
                   POST_EXEC_SCRIPT nopost.sh
                   PROPERTIES TIMEOUT 300 )
endif()

atlas_add_executable( TrigConfCoolFix
                      src/apps/CoolFix.cxx src/apps/TrigConfCoolWriter.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} CxxUtils TrigConfBase TrigConfHLTData TrigConfL1Data TrigConfStorage )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( scripts/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
