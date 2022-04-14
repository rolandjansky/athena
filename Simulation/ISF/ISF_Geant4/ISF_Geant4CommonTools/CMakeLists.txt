################################################################################
# Package: ISF_Geant4CommonTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_Geant4CommonTools )

# Component(s) in the package:
atlas_add_library( ISF_Geant4CommonTools
                   OBJECT
                   NO_PUBLIC_HEADERS
                   src/*.cxx
                   src/components/*.cxx
                   PRIVATE_LINK_LIBRARIES GaudiKernel AthenaBaseComps ISF_Event ISF_InterfacesLib G4AtlasToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

