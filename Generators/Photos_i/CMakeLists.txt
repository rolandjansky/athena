################################################################################
# Package: Photos_i
################################################################################

# Declare the package name:
atlas_subdir( Photos_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Generators/GeneratorModules
                          Generators/Tauola_i
                          PRIVATE
                          Control/AthenaKernel
                          Generators/GeneratorObjects
                          Generators/GeneratorUtils
                          Generators/TruthUtils )

# External dependencies:
find_package( CLHEP )
find_package( HepMC COMPONENTS HepMC HepMCfio )
find_package( Photos )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Photos_iLib
                   src/Photos.cxx
                   src/Ffpholun.cxx
                   src/Ffphot.cxx
                   src/gffpholun.F
                   src/gffphot.F
                   PUBLIC_HEADERS Photos_i
                   INCLUDE_DIRS ${PHOTOS_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${PHOTOS_LIBRARIES} GaudiKernel GeneratorModulesLib Tauola_iLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthenaKernel GeneratorObjects TruthUtils )

atlas_add_component( Photos_i
                     src/components/*.cxx
                     INCLUDE_DIRS ${PHOTOS_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${PHOTOS_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel GeneratorModulesLib Tauola_iLib AthenaKernel GeneratorObjects TruthUtils Photos_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

