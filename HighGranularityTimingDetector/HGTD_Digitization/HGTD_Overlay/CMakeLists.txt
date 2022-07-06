# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HGTD_Overlay )

# External dependencies:
find_package( CLHEP )
find_package( GTest )
find_package( GMock )

# Unit Tests
atlas_add_test( HGTD_Overlay_test
                SOURCES test/HGTD_Overlay_test.cxx src/HGTD_Overlay.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES ${GTEST_LIBRARIES} CxxUtils IDC_OverlayBase HGTD_RawData StoreGateLib TestTools
                POST_EXEC_SCRIPT nopost.sh )

# Component(s) in the package:
atlas_add_component( HGTD_Overlay
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel AtlasHepMCLib GaudiKernel IDC_OverlayBase HGTD_RawData )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Configuration tests
# TODO: to be enabled once we have reasonable inputs
# atlas_add_test( HGTD_OverlayConfig_test
#                 SCRIPT test/HGTD_OverlayConfig_test.py
#                 POST_EXEC_SCRIPT nopost.sh )
