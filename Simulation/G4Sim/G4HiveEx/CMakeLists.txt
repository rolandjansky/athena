################################################################################
# Package: G4HiveEx
################################################################################

# Declare the package name:
atlas_subdir( G4HiveEx )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          GaudiKernel
                          Generators/GeneratorObjects )

# External dependencies:
find_package( Geant4 )

# Component(s) in the package:
atlas_add_component( G4HiveEx
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} AthenaKernel AthenaBaseComps GeneratorObjects )

# Install files from the package:
atlas_install_joboptions( share/*.py )

