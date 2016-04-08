################################################################################
# Package: CavernBkgGenerator
################################################################################

# Declare the package name:
atlas_subdir( CavernBkgGenerator )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel
                          Generators/GeneratorModules )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS minicern )

# tag $(as_needed_tag) was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( CavernBkgGeneratorLib
                   src/GenerateCavernBkg.cxx
                   src/MuonBackgroundEventParser.cxx
                   src/MuonBackgroundConverter.cxx
                   src/gbkgout.F
                   src/cavbkg.F
                   src/atlrgeom.F
                   src/vdist.F
                   PUBLIC_HEADERS CavernBkgGenerator
                   INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel ${ROOT_LIBRARIES} GeneratorModulesLib )

atlas_add_component( CavernBkgGenerator
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GaudiKernel GeneratorModulesLib CavernBkgGeneratorLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

