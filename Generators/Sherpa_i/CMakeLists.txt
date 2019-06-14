################################################################################
# Package: Sherpa_i
################################################################################

#set( SHERPA_VERSION 2.2.5 )
#set( SHERPA_ROOT ${LCG_RELEASE_DIR}/MCGenerators/sherpa/${SHERPA_VERSION}/${LCG_PLATFORM} )
#set( SHERPA_ROOT /cvmfs/sft.cern.ch/lcg/releases/LCG_88/MCGenerators/sherpa/${SHERPA_VERSION}/x86_64-slc6-gcc62-opt )
#set( SHERPA_ROOT /afs/cern.ch/user/f/fsiegert/public/atlas/rel21/sherpa-installation/SHERPA-MC-${SHERPA_VERSION} )

# Declare the package name:
atlas_subdir( Sherpa_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   Generators/GeneratorModules
   Generators/TruthUtils
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( Sherpa COMPONENTS SherpaTools SherpaInitialization )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( Sherpa_i
   Sherpa_i/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   ${SHERPA_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${SHERPA_LIBRARIES}
   GaudiKernel GeneratorModulesLib TruthUtils AthenaBaseComps AthenaKernel )

add_definitions(-DSHERPA_ROOT="${SHERPA_LCGROOT}")

atlas_install_joboptions( share/common/*.py )

set( SherpaEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of SherpaEnvironment.cmake" )
find_package( SherpaEnvironment )
