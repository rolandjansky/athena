################################################################################
# Package: ISF_Geant4CommonTools
################################################################################

# Declare the package name:
atlas_subdir( ISF_Geant4CommonTools )

# Component(s) in the package:
atlas_add_component( ISF_Geant4CommonTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps ISF_Event ISF_InterfacesLib G4AtlasToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

