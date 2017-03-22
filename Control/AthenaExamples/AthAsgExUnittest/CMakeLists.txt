################################################################################
# Package: MyPackage
################################################################################

# Declare the package name:
atlas_subdir( AthAsgExUnittest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthAnalysisBaseComps
                          Control/AthToolSupport/AsgTools
                          TestPolicy )

# Library in the package:
atlas_add_library( AthAsgExUnittestLib
                   MyPackage/*.h src/*.cxx
                   src/components/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS MyPackage
                   LINK_LIBRARIES GaudiKernel AthAnalysisBaseComps AsgTools )
#atlas_add_component( MyPackage
#                     MyPackage/*.h src/*.cxx
#                     src/components/*.cxx
#                     Root/*.cxx
#                     LINK_LIBRARIES GaudiKernel AthAnalysisBaseComps 
#                                    AsgTools MyPackageLib )

atlas_add_test( gt_AthAsgExUnittest
  SOURCES test/gt_AthAsgExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthAsgExUnittestLib )


# Install files from the package:
#atlas_install_headers( MyPackage )
#atlas_install_joboptions( share/*.py )
#atlas_install_runtime( test/AthExHelloWorld_TestConfiguration.xml )

