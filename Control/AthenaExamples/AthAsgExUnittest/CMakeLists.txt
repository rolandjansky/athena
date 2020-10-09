# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthAsgExUnittest )

# Necessary external(s).
find_package( GTest )
find_package( GMock )

# Libraries in the package:
atlas_add_library( AthAsgExUnittestLib
   AthAsgExUnittest/*.h Root/*.cxx
   PUBLIC_HEADERS AthAsgExUnittest
   LINK_LIBRARIES AsgTools )

atlas_add_component( AthAsgExUnittest
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthAnalysisBaseCompsLib AsgTools AthAsgExUnittestLib )

# Add tests:
atlas_add_test( gt_AthAsgExUnittest
  SOURCES test/gt_AthAsgExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AsgTools AthAnalysisBaseCompsLib
  AthAsgExUnittestLib )

atlas_add_test( gt_AthExUnittest
  SOURCES test/gt_AthExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthenaBaseComps
  AthAsgExUnittestLib )

atlas_add_test( gt_MockxAODJet
  SOURCES test/gt_MockxAODJet.cxx
  INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
  LINK_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} xAODJet )

atlas_add_test( gt_xAODJet
  SOURCES test/gt_xAODJet.cxx
  INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
  LINK_LIBRARIES ${GTEST_LIBRARIES} xAODJet )
