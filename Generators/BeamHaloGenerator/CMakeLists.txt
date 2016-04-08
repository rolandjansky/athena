################################################################################
# Package: BeamHaloGenerator
################################################################################

# Declare the package name:
atlas_subdir( BeamHaloGenerator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Generators/GeneratorModules
                          PRIVATE
                          Control/AthenaKernel )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( BeamHaloGeneratorLib
                   src/*.cxx
                   PUBLIC_HEADERS BeamHaloGenerator
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel )

atlas_add_component( BeamHaloGenerator
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel GeneratorModulesLib AthenaKernel BeamHaloGeneratorLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

