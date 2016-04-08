################################################################################
# Package: Reldis_i
################################################################################

# Declare the package name:
atlas_subdir( Reldis_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Generators/GeneratorModules
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( Reldis_i
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} GeneratorModulesLib GaudiKernel )

# Install files from the package:
atlas_install_headers( Reldis_i )
atlas_install_joboptions( share/*.py )

