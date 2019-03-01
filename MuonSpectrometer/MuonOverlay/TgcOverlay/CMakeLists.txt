################################################################################
# Package: TgcOverlay
################################################################################

# Declare the package name:
atlas_subdir( TgcOverlay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonOverlay/MuonOverlayBase
                          PRIVATE
                          AtlasTest/TestTools
                          Control/AthenaKernel
                          Control/StoreGate
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonIdHelpers )

#External dependencies:
find_package( GTest )

atlas_add_test( TgcOverlay_test
                SOURCES src/*.cxx test/TgcOverlay_test.cxx
                INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
                LINK_LIBRARIES GaudiKernel TestTools MuonDigitContainer MuonOverlayBase AthenaKernel StoreGateLib SGtests MuonIdHelpersLib ${GTEST_LIBRARIES}
                EXTRA_PATTERNS "[0-9]+ ms")

# Component(s) in the package:
atlas_add_component( TgcOverlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonDigitContainer MuonOverlayBase AthenaKernel StoreGateLib SGtests MuonIdHelpersLib )

# Install files from the package:
atlas_install_headers( TgcOverlay )
atlas_install_python_modules( python/*.py )
