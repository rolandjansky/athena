################################################################################
# Package: Sherpa_i
################################################################################

# Declare the package name:
atlas_subdir( Sherpa_i )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Generators/GeneratorModules
                          Generators/AtlasHepMC
                          Generators/TruthUtils
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel )

# Set the version of Sherpa to use:
#set( SHERPA_VERSION 2.2.2 )
#set( SHERPA_ROOT
#   ${LCG_RELEASE_DIR}/MCGenerators/sherpa/${SHERPA_VERSION}/${ATLAS_PLATFORM} )

# External dependencies:
find_package( CLHEP )
find_package( Sherpa COMPONENTS SherpaTools )

# Sherpa is linked against HepMC3, so set that up for the runtime environment.
find_package( hepmc3 )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_component( Sherpa_i
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}  ${SHERPA_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib ${SHERPA_LIBRARIES} GaudiKernel GeneratorModulesLib TruthUtils AthenaBaseComps AthenaKernel )

# Install files from the package:
atlas_install_headers( Sherpa_i )

# Install JOs common fragments from the package share directory
atlas_install_joboptions( share/common/*.py )

