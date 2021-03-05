################################################################################
# Package: ISF_ActsServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_ActsServices )

find_package( Acts COMPONENTS Core PluginJson )

# Component(s) in the package:
atlas_add_component( ISF_ActsServices
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasDetDescr GaudiKernel
                     ISF_InterfacesLib TrkTrack ISF_Event ActsCore
                     ActsPluginJson)

# Install files from the package:
atlas_install_python_modules( python/*.py )

