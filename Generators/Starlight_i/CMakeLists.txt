################################################################################
# Package: Starlight_i
################################################################################

# Declare the package name:
atlas_subdir( Starlight_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorModules
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorUtils
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( Starlight )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Starlight_iLib
                   src/*.cxx
                   PUBLIC_HEADERS Starlight_i
                   INCLUDE_DIRS ${STARLIGHT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${STARLIGHT_LIBRARIES} GeneratorModulesLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel TruthUtils )

atlas_add_component( Starlight_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${STARLIGHT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} ${STARLIGHT_LIBRARIES} GeneratorModulesLib AthenaKernel StoreGateLib SGtests GaudiKernel TruthUtils Starlight_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

