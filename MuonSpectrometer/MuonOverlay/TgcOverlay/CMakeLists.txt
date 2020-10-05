################################################################################
# Package: TgcOverlay
################################################################################

# Declare the package name:
atlas_subdir( TgcOverlay )

#External dependencies:
find_package( GTest )

atlas_add_test( TgcOverlay_test
                SOURCES src/*.cxx test/TgcOverlay_test.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES GaudiKernel TestTools MuonDigitContainer MuonOverlayBase AthenaKernel StoreGateLib SGtests MuonIdHelpersLib ${GTEST_LIBRARIES} )

# Component(s) in the package:
atlas_add_component( TgcOverlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonDigitContainer MuonOverlayBase AthenaKernel StoreGateLib SGtests MuonIdHelpersLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
