################################################################################
# Package: Hijing_i
################################################################################

# Declare the package name:
atlas_subdir( Hijing_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
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

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Hijing_iLib
   Hijing_i/*.h src/*.cxx src/*.F src/Hijing_modified/*.f
   PUBLIC_HEADERS Hijing_i
   INCLUDE_DIRS ${HIJING_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${HEPMC_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   PRIVATE_DEFINITIONS -DHIJINGVERSION=\"${HIJING_VERSION}\"
   LINK_LIBRARIES ${HIJING_LIBRARIES} ${CLHEP_LIBRARIES}
   GeneratorFortranCommonLib GeneratorModulesLib StoreGateLib
   PRIVATE_LINK_LIBRARIES ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel
   GeneratorObjects TruthUtils )

atlas_add_component( Hijing_i
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel Hijing_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
