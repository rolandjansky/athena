################################################################################
# Package: Sherpa_i
################################################################################

# Declare the package name:
atlas_subdir( Sherpa_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   Generators/EvgenProdTools
   Generators/GeneratorModules
   Generators/TruthUtils
   PRIVATE
   Control/AthenaBaseComps
   Control/AthenaKernel )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( OpenLoops )
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

# Install file(s) from the package.
atlas_install_joboptions( share/common/*.py )

# Set (a) Sherpa specific environment variable(s).
set( SherpaEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of SherpaEnvironment.cmake" )
find_package( SherpaEnvironment )
