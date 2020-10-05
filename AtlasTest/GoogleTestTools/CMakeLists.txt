# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GoogleTestTools )

# External package dependencies:
find_package( GTest )

# In standalone mode we just use the headers from the package. While in
# offline mode we build a proper library.
if( XAOD_STANDALONE )
  atlas_add_library( GoogleTestTools
    GoogleTestTools/*.h
    INTERFACE
    PUBLIC_HEADERS GoogleTestTools )
else()
  atlas_add_library( GoogleTestTools
    GoogleTestTools/*.h src/*.cxx
    PUBLIC_HEADERS GoogleTestTools
    INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
    LINK_LIBRARIES ${GTEST_LIBRARIES} GaudiKernel )
endif()

# Unit tests for InitGaudiGoogleTest:
atlas_add_test( GoogleTestToolsTests
  SOURCES test/gt_GoogleTestTools.cxx
  LINK_LIBRARIES GoogleTestTools )
