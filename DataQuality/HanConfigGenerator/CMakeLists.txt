################################################################################
# Package: HanConfigGenerator
################################################################################

# Declare the package name:
atlas_subdir( HanConfigGenerator )

# External dependencies:
find_package( ROOT COMPONENTS Graf Gpad Cint Core Tree MathCore Hist RIO pthread )


atlas_add_executable( hcg
                      src/hcg.cxx src/addnode.cxx src/utils.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} )


