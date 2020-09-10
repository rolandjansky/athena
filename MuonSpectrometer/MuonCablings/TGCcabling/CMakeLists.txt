################################################################################
# Package: TGCcabling
################################################################################

# Declare the package name:
atlas_subdir( TGCcabling )

# Component(s) in the package:
atlas_add_library( TGCcablingLib
                   src/*.cxx
                   PUBLIC_HEADERS TGCcabling
                   LINK_LIBRARIES GaudiKernel TGCcablingInterfaceLib StoreGateLib SGtests MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES PathResolver )

atlas_add_component( TGCcabling
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TGCcablingInterfaceLib StoreGateLib SGtests MuonIdHelpersLib PathResolver TGCcablingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.db )

