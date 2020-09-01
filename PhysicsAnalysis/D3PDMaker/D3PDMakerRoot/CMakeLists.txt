################################################################################
# Package: D3PDMakerRoot
################################################################################

# Declare the package name:
atlas_subdir( D3PDMakerRoot )

# External dependencies:
find_package( ROOT COMPONENTS RIO Tree Core MathCore Hist pthread )

# Component(s) in the package:
atlas_add_component( D3PDMakerRoot
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel D3PDMakerInterfaces )

