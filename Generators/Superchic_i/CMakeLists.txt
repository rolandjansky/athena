# $Id: CMakeLists.txt 762209 2016-07-15 14:15:34Z krasznaa $
################################################################################
# Package: Superchic_i
################################################################################

# Declare the package name:
atlas_subdir( Superchic_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Generators/GeneratorFortranCommon
   Generators/GeneratorModules
   PRIVATE
   Control/AthenaKernel
   Control/StoreGate
   GaudiKernel
   Generators/GeneratorUtils
   Generators/TruthUtils
   Projects/AthGeneration
   Projects/Athena 
                  )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )
find_package( Apfel )
find_package( Lhapdf )
find_package( Superchic )

atlas_disable_as_needed()
# Component(s) in the package:
atlas_add_library( Superchic_iLib
   Superchic_i/*.h src/*.cxx src/*.F
   PUBLIC_HEADERS Superchic_i
   INCLUDE_DIRS ${SUPERCHIC_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${SUPERCHIC_LIBRARIES}  GeneratorModulesLib
   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel
   GaudiKernel TruthUtils )

atlas_add_component( Superchic_i
   src/components/*.cxx
   INCLUDE_DIRS ${SUPERCHIC_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
   LINK_LIBRARIES ${SUPERCHIC_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorFortranCommonLib GeneratorModulesLib AthenaKernel StoreGateLib SGtests GaudiKernel GeneratorObjects TruthUtils Superchic_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
