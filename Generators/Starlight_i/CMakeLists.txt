# $Id: CMakeLists.txt 762209 2016-07-15 14:15:34Z krasznaa $
################################################################################
# Package: Starlight_i
################################################################################

# Declare the package name:
atlas_subdir( Starlight_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Generators/GeneratorModules
   PRIVATE
   Control/AthenaKernel
   Control/StoreGate
   GaudiKernel
   Generators/GeneratorUtils
   Generators/TruthUtils )

# Set the version of Starlight to use:
set( STARLIGHT_VERSION r193 )
set( STARLIGHT_ROOT
   ${LCG_RELEASE_DIR}/MCGenerators/starlight/${STARLIGHT_VERSION}/${ATLAS_PLATFORM} )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( Starlight )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Starlight_iLib
   Starlight_i/*.h src/*.cxx
   PUBLIC_HEADERS Starlight_i
   INCLUDE_DIRS ${STARLIGHT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${STARLIGHT_LIBRARIES} GeneratorModulesLib StoreGateLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel
   GaudiKernel TruthUtils )

atlas_add_component( Starlight_i
   src/components/*.cxx
   LINK_LIBRARIES Starlight_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
