################################################################################
# Package: TruthUtils
################################################################################

# Declare the package name:
atlas_subdir( TruthUtils )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( FastJet )
find_package( HEPUtils )
find_package( HepMC )
find_package( MCUtils )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TruthUtils
                   Root/*.cxx
                   PUBLIC_HEADERS TruthUtils
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPUTILS_INCLUDE_DIRS} ${MCUTILS_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${HEPUTILS_LIBRARIES} ${MCUTILS_LIBRARIES} ${HEPMC_LIBRARIES} ${FASTJET_LIBRARIES} )

set( TruthUtilsEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of TruthUtilsEnvironment.cmake" )
find_package( TruthUtilsEnvironment )


