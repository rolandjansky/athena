# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BLM_G4_SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( BLM_G4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} ${GTEST_LIBRARIES} G4AtlasToolsLib InDetSimEvent MCTruth StoreGateLib )

atlas_add_component( BLM_G4_SD
                     src/components/*.cxx
                     LINK_LIBRARIES BLM_G4_SDLib )

# Test(s) in the package:
atlas_add_test( BLM_G4_SDToolConfig_test
                SCRIPT test/BLM_G4_SDToolConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( BLMSensorSD_gtest
                SOURCES
                test/BLMSensorSD_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${GTEST_LIBRARIES} AtlasHepMCLib G4AtlasToolsLib InDetSimEvent MCTruth StoreGateLib TestTools BLM_G4_SDLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/optionForTest.txt )
