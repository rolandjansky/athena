################################################################################
# Package: PythiaRhad_i
################################################################################

# Declare the package name:
atlas_subdir( PythiaRhad_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Generators/GeneratorModules
                          Generators/Pythia_i
                          Generators/Tauola_i
                          PRIVATE
                          Control/AthenaKernel
                          Generators/GeneratorUtils
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( PythiaRhad_iLib
                   src/PythiaRhad.cxx
                   src/PythiaRhadModified/pyrhad.F
                   src/PythiaRhadModified/pysthad.F
                   src/PythiaRhadModified/pyprep.F
                   PUBLIC_HEADERS PythiaRhad_i
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES GaudiKernel GeneratorModulesLib Pythia_iLib Tauola_iLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel TruthUtils )

atlas_add_component( PythiaRhad_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel GeneratorModulesLib Pythia_iLib Tauola_iLib AthenaKernel TruthUtils PythiaRhad_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

