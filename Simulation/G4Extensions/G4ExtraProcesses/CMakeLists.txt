################################################################################
# Package: G4ExtraProcesses
################################################################################

# Declare the package name:
atlas_subdir( G4ExtraProcesses )

# External dependencies:
find_package( Geant4 )

# Component(s) in the package:
atlas_add_component( G4ExtraProcesses
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} GaudiKernel AthenaBaseComps G4AtlasInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

