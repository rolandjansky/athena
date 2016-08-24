################################################################################
# Package: Pyquench_i
################################################################################

# Declare the package name:
atlas_subdir( Pyquench_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorModules
                          Generators/Pythia_i
                          Generators/Tauola_i
                          PRIVATE
                          Control/AthenaKernel
                          GaudiKernel
                          Generators/GeneratorUtils
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Pyquench_iLib
                   src/*.cxx
                   src/*.f
                   PUBLIC_HEADERS Pyquench_i
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GeneratorModulesLib Pythia_iLib Tauola_iLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel TruthUtils )

atlas_add_component( Pyquench_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorModulesLib Pythia_iLib Tauola_iLib AthenaKernel GaudiKernel TruthUtils Pyquench_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

