# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthAsgExUnittest )

# Libraries in the package:
atlas_add_library( AthAsgExUnittestLib
                   MyPackage/*.h src/*.cxx
                   Root/*.cxx
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES GaudiKernel AsgTools AthAnalysisBaseCompsLib )

atlas_add_component( AthAsgExUnittest
                     src/components/*.cxx
                     LINK_LIBRARIES AthAsgExUnittestLib )

# Add tests:
atlas_add_test( gt_AthAsgExUnittest
  SOURCES test/gt_AthAsgExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthAsgExUnittestLib )

atlas_add_test( gt_AthExUnittest
  SOURCES test/gt_AthExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthenaBaseComps AsgTools )

atlas_add_test( gt_MockxAODJet
  SOURCES test/gt_MockxAODJet.cxx
  LINK_LIBRARIES xAODJet GoogleTestTools )

atlas_add_test( gt_xAODJet
  SOURCES test/gt_xAODJet.cxx
  LINK_LIBRARIES xAODJet GoogleTestTools )
