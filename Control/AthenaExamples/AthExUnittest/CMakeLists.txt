################################################################################
# Package: MyPackage
################################################################################

# Declare the package name:
atlas_subdir( AthExUnittest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
			  Control/AthenaBaseComps
                          TestPolicy )

# Library in the package:
# atlas_add_library( AthExUnittestLib
#                    MyPackage/*.h src/*.cxx
#                    src/components/*.cxx
#                    Root/*.cxx
#                    PUBLIC_HEADERS MyPackage
#                    LINK_LIBRARIES GaudiKernel AthenaBaseCompsLib )

atlas_add_component( AthExUnittest
                     MyPackage/*.h src/*.cxx
                     src/components/*.cxx
                     Root/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps )

atlas_add_test( gt_AthExUnittest
  SOURCES test/gt_AthExUnittest.cxx
  LINK_LIBRARIES GaudiKernel GoogleTestTools AthenaBaseComps )


