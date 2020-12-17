# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_G4_SD )

# External dependencies:
find_package( Geant4 )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( TRT_G4_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                   LINK_LIBRARIES ${GEANT4_LIBRARIES} ${GTEST_LIBRARIES} AthenaKernel CxxUtils G4AtlasToolsLib InDetSimEvent MCTruth StoreGateLib TRT_G4Utilities )

atlas_add_component( TRT_G4_SD
                     src/components/*.cxx
                     LINK_LIBRARIES TRT_G4_SDLib )

# Test(s) in the package:
atlas_add_test( TRT_G4_SDToolConfig_test
                SCRIPT test/TRT_G4_SDToolConfig_test.py
                PROPERTIES TIMEOUT 300 )

atlas_add_test( TRTSensitiveDetector_gtest
                SOURCES
                test/TRTSensitiveDetector_gtest.cxx
                INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GEANT4_LIBRARIES} ${GTEST_LIBRARIES} CxxUtils G4AtlasToolsLib InDetSimEvent MCTruth StoreGateLib TestTools TRT_G4_SDLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/optionForTest.txt )
