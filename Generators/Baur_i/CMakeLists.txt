################################################################################
# Package: Baur_i
################################################################################

# Declare the package name:
atlas_subdir( Baur_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorModules
                          PRIVATE
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorFortranCommon
                          Generators/TruthUtils )

# External dependencies:
find_package( BaurMC )
find_package( CLHEP )
find_package( Lhapdf )
find_package( Pythia6 )

# tag allowUndefined was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( Baur_iLib
                   src/*.cxx
                   src/*.F
                   PUBLIC_HEADERS Baur_i
                   INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${BAURMC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${BAURMC_LIBRARIES} ${CLHEP_LIBRARIES} GeneratorModulesLib StoreGateLib SGtests GeneratorFortranCommonLib
                   PRIVATE_LINK_LIBRARIES AthenaKernel GaudiKernel TruthUtils )

atlas_add_component( Baur_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${PYTHIA6_INCLUDE_DIRS} ${LHAPDF_INCLUDE_DIRS} ${BAURMC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${PYTHIA6_LIBRARIES} ${LHAPDF_LIBRARIES} ${BAURMC_LIBRARIES} ${CLHEP_LIBRARIES} GeneratorModulesLib AthenaKernel StoreGateLib SGtests GaudiKernel GeneratorFortranCommonLib TruthUtils Baur_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

