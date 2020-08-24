################################################################################
# Package: ISF_FatrasServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_FatrasServices )

# Component(s) in the package:
atlas_add_component( ISF_FatrasServices
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasDetDescr GaudiKernel ISF_InterfacesLib TrkTrack ISF_Event )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

