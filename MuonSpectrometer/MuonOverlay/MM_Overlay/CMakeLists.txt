################################################################################
# Package: MM_Overlay
################################################################################

# Declare the package name:
atlas_subdir( MM_Overlay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonOverlay/MuonOverlayBase
                          PRIVATE
                          AtlasTest/TestTools
                          Control/StoreGate
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonIdHelpers )

#External dependencies:
find_package( GTest )

# Component(s) in the package:
atlas_add_component( MM_Overlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonDigitContainer MuonOverlayBase StoreGateLib SGtests MuonIdHelpersLib )

# Install files from the package:
atlas_install_headers( MM_Overlay )
atlas_install_python_modules( python/*.py )
