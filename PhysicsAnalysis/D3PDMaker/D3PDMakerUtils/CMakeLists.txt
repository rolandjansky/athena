################################################################################
# Package: D3PDMakerUtils
################################################################################

# Declare the package name:
atlas_subdir( D3PDMakerUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( D3PDMakerUtils
                   src/*.cxx
                   PUBLIC_HEADERS D3PDMakerUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel AthLinks SGTools GaudiKernel StoreGateLib SGtests D3PDMakerInterfaces
                   PRIVATE_LINK_LIBRARIES AthContainersInterfaces ${ROOT_LIBRARIES} )

