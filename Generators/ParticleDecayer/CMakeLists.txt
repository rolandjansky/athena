################################################################################
# Package: ParticleDecayer
################################################################################

# Declare the package name:
atlas_subdir( ParticleDecayer )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Generators/GeneratorModules
                          PRIVATE
                          GaudiKernel
                          Generators/GeneratorObjects )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )
find_package( HepPDT )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( ParticleDecayer
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GeneratorModulesLib GaudiKernel GeneratorObjects )

# Install files from the package:
atlas_install_headers( ParticleDecayer )
atlas_install_joboptions( share/*.py )

