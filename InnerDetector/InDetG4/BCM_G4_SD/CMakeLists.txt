# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BCM_G4_SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( BCM_G4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} ${GTEST_LIBRARIES} AthenaKernel G4AtlasToolsLib InDetSimEvent MCTruth StoreGateLib )

atlas_add_component( BCM_G4_SD
                     src/components/*.cxx
                     LINK_LIBRARIES BCM_G4_SDLib )

# Test(s) in the package:
atlas_add_test( BCM_G4_SDToolConfig_test
                SCRIPT test/BCM_G4_SDToolConfig_test.py
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( BCMSensorSD_gtest
                SOURCES
                test/BCMSensorSD_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${GTEST_LIBRARIES} G4AtlasToolsLib InDetSimEvent MCTruth StoreGateLib TestTools BCM_G4_SDLib
                POST_EXEC_SCRIPT nopost.sh )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/optionForTest.txt )
