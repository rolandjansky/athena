################################################################################
# Package: MDT_Response
################################################################################

# Declare the package name:
atlas_subdir( MDT_Response )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Simulation/Tools/AtlasCLHEP_RandomGenerators )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( MDT_Response
                   src/Amplifier.cxx
                   src/MDT_Response.cxx
                   PUBLIC_HEADERS MDT_Response
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasCLHEP_RandomGenerators
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_executable( MDT_ResponseTest
                      src/MDT_ResponseTest.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasCLHEP_RandomGenerators MDT_Response )

