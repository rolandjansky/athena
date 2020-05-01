################################################################################
# Package: AtlasCLHEP_RandomGenerators
################################################################################

# Declare the package name:
atlas_subdir( AtlasCLHEP_RandomGenerators )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( dSFMT )

# Component(s) in the package:
atlas_add_library( AtlasCLHEP_RandomGenerators
                   src/*.cxx
                   PUBLIC_HEADERS AtlasCLHEP_RandomGenerators
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${DSFMT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${DSFMT_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel )

