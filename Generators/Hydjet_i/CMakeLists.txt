################################################################################
# Package: Hydjet_i
################################################################################

# Declare the package name:
atlas_subdir( Hydjet_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorFortranCommon
                          Generators/GeneratorModules
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorObjects
                          Generators/GeneratorUtils
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( Hijing )
find_package( Hydjet )
find_package( Pyquen )
find_package( Pythia6 )
find_package( ROOT COMPONENTS minicern Core Tree MathCore Hist RIO pthread )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Hydjet_iLib
                   src/*.cxx
                   src/*.F
                   PUBLIC_HEADERS Hydjet_i
                   INCLUDE_DIRS ${PYQUEN_INCLUDE_DIRS} ${PYTHIA6_INCLUDE_DIRS} ${HYDJET_INCLUDE_DIRS} ${HIJING_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${PYQUEN_LIBRARIES} ${PYTHIA6_LIBRARIES} ${HYDJET_LIBRARIES} ${HIJING_LIBRARIES} ${ROOT_LIBRARIES} GeneratorFortranCommonLib GeneratorModulesLib StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel GeneratorObjects TruthUtils )

atlas_add_component( Hydjet_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${PYQUEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${PYTHIA6_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${HIJING_INCLUDE_DIRS} ${HYDJET_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${PYQUEN_LIBRARIES} ${CLHEP_LIBRARIES} ${PYTHIA6_LIBRARIES} ${HEPMC_LIBRARIES} ${HIJING_LIBRARIES} ${HYDJET_LIBRARIES} ${ROOT_LIBRARIES} GeneratorFortranCommonLib GeneratorModulesLib AthenaKernel StoreGateLib SGtests GaudiKernel GeneratorObjects TruthUtils Hydjet_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

