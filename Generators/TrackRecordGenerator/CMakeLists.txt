################################################################################
# Package: TrackRecordGenerator
################################################################################

# Declare the package name:
atlas_subdir( TrackRecordGenerator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorModules
                          Generators/AtlasHepMC
                          PRIVATE
                          GaudiKernel
                          Simulation/G4Sim/TrackRecord )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrackRecordGeneratorLib
                   src/TrackRecordGenerator.cxx
                   PUBLIC_HEADERS TrackRecordGenerator
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrackRecordLib )

atlas_add_component( TrackRecordGenerator
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib GeneratorModulesLib GaudiKernel TrackRecordGeneratorLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

