################################################################################
# Package: CosmicGenerator
################################################################################

# Declare the package name:
atlas_subdir( CosmicGenerator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Generators/AtlasHepMC
                          Generators/GeneratorModules
                          Generators/AtlasHepMC
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( CosmicGeneratorLib
                   src/cosmic2.f
                   src/CosmicGun.cxx
                   src/CosmicGenerator.cxx
                   PUBLIC_HEADERS CosmicGenerator
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaKernel GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_component( CosmicGenerator
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaKernel GeneratorModulesLib GaudiKernel CosmicGeneratorLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.txt share/*.py )

