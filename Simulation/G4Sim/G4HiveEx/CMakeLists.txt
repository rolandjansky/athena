################################################################################
# Package: G4HiveEx
################################################################################

# Declare the package name:
atlas_subdir( G4HiveEx )

# External dependencies:
find_package( Geant4 )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( G4HiveEx
                   src/*.cxx
                   src/components/*.cxx
                   OBJECT
                   NO_PUBLIC_HEADERS
                   PRIVATE_INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel AthenaBaseComps GeneratorObjects )

# Install files from the package:
atlas_install_joboptions( share/*.py )
