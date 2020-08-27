# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GoogleTestTools )

# External package dependencies:
find_package( GMock )

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
    INCLUDE_DIRS ${GMOCK_INCLUDE_DIRS}
    LINK_LIBRARIES ${GMOCK_LIBRARIES}
    PRIVATE_LINK_LIBRARIES GaudiKernel )
endif()

# Unit tests for InitGaudiGoogleTest:
atlas_add_test( GoogleTestToolsTests
  SOURCES test/gt_GoogleTestTools.cxx
  INCLUDE_DIRS ${GMOCK_INCLUDE_DIRS} 
  LINK_LIBRARIES ${GMOCK_LIBRARIES} GaudiKernel GoogleTestTools )
