# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ZDC_SD )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( GTest )

# Component(s) in the package:
atlas_add_library( ZDC_SDLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${GEANT4_LIBRARIES} ${GTEST_LIBRARIES} G4AtlasToolsLib StoreGateLib ZDC_SimEvent )

atlas_add_component( ZDC_SD
                     src/components/*.cxx
                     LINK_LIBRARIES ZDC_SDLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/optionForTest.txt )

# Tests in the package:
atlas_add_test( ZDC_StripSD_gtest
                SOURCES test/ZDC_StripSD_gtest.cxx
                LINK_LIBRARIES TestTools ZDC_SDLib )

atlas_add_test( ZDC_PixelSD_gtest
                SOURCES test/ZDC_PixelSD_gtest.cxx
                LINK_LIBRARIES TestTools ZDC_SDLib )
