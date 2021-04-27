################################################################################
# Package: STGC_Overlay
################################################################################

# Declare the package name:
atlas_subdir( STGC_Overlay )

#External dependencies:
find_package( GTest )

# Component(s) in the package:
atlas_add_component( STGC_Overlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonDigitContainer MuonOverlayBase StoreGateLib MuonIdHelpersLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
