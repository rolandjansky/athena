################################################################################
# Package: ISF_ParSimServices
################################################################################

# Declare the package name:
atlas_subdir( ISF_ParSimServices )

# Component(s) in the package:
atlas_add_component( ISF_ParSimServices
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel ISF_InterfacesLib ISF_ParSimInterfacesLib TrkDetDescrInterfaces ISF_Event )

# Install files from the package:
atlas_install_python_modules( python/*.py )

