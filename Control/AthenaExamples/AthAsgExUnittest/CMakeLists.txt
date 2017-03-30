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
                          TestPolicy )

# Library in the package:
atlas_add_library( AthAsgExUnittestLib
                   MyPackage/*.h src/*.cxx
                   src/components/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS MyPackage
                   LINK_LIBRARIES GaudiKernel AthAnalysisBaseCompsLib AsgTools )

atlas_add_component( AthAsgExUnittest
                     MyPackage/*.h src/*.cxx
                     src/components/*.cxx
                     Root/*.cxx
                     LINK_LIBRARIES GaudiKernel AthAnalysisBaseComps 
                                    AsgTools AthAsgExUnittestLib )

atlas_add_test( gt_AthAsgExUnittest
  SOURCES test/gt_AthAsgExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthAsgExUnittestLib )

atlas_add_test( gt_AthExUnittest
  SOURCES test/gt_AthExUnittest.cxx
#  LINK_LIBRARIES GaudiKernel GoogleTestTools AthenaBaseComps )
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthAnalysisBaseComps )

atlas_add_test( gt_MockxAODJet
  SOURCES test/gt_MockxAODJet.cxx
  LINK_LIBRARIES xAODJet GoogleTestTools )

atlas_add_test( gt_xAODJet
  SOURCES test/gt_xAODJet.cxx
  LINK_LIBRARIES xAODJet GoogleTestTools )


