
#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

################################################################################
# Package: MyPackage
################################################################################

# Declare the package name:
atlas_subdir( AthAsgExUnittest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
			  Control/AthenaBaseComps
                          Control/AthAnalysisBaseComps
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODJet
                          AtlasTest/GoogleTestTools )

# Libraries in the package:
atlas_add_library( AthAsgExUnittestLib
                   MyPackage/*.h src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS AthAsgExUnittest
                   LINK_LIBRARIES GaudiKernel AsgTools AthAnalysisBaseCompsLib )
atlas_add_component( AthAsgExUnittest
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AsgTools AthAsgExUnittestLib )

# Add tests:
atlas_add_test( gt_AthAsgExUnittest
  SOURCES test/gt_AthAsgExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthAsgExUnittestLib )

atlas_add_test( gt_AthExUnittest
  SOURCES test/gt_AthExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthenaBaseComps )

atlas_add_test( gt_MockxAODJet
  SOURCES test/gt_MockxAODJet.cxx
  LINK_LIBRARIES xAODJet GoogleTestTools )

atlas_add_test( gt_xAODJet
  SOURCES test/gt_xAODJet.cxx
  LINK_LIBRARIES xAODJet GoogleTestTools )


