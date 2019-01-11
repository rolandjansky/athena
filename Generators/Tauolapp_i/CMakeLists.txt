################################################################################
# Package: Tauolapp_i
################################################################################

# Declare the package name:
atlas_subdir( Tauolapp_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          GaudiKernel
                          Generators/GeneratorObjects )

# Set the version of Tauolapp to use:
#set( TAUOLAPP_VERSION 1.1.6 )
#set( TAUOLAPP_ROOT
#   ${LCG_RELEASE_DIR}/MCGenerators/tauola++/${TAUOLAPP_VERSION}/${ATLAS_PLATFORM} )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( Tauolapp )

# Component(s) in the package:
atlas_add_component( Tauolapp_i
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TAUOLAPP_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TAUOLAPP_LIBRARIES} ${HEPMC_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaPoolUtilities EventInfo GaudiKernel GeneratorObjects )

# Install files from the package:
atlas_install_headers( Tauolapp_i )
atlas_install_joboptions( share/common/*.py )

set( TauolappEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of TauolappEnvironment.cmake" )
find_package( TauolappEnvironment )


