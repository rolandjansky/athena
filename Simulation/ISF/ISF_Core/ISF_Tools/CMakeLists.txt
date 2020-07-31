################################################################################
# Package: ISF_Tools
################################################################################

# Declare the package name:
atlas_subdir( ISF_Tools )

# Component(s) in the package:
atlas_add_component( ISF_Tools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AtlasDetDescr BarcodeInterfacesLib GeneratorObjects ISF_Event ISF_InterfacesLib TrackRecordLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

