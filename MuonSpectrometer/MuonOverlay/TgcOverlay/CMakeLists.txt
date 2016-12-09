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
                          Control/AthenaKernel
			  Control/CxxUtils  
                          Control/StoreGate
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimData )

# Component(s) in the package:
atlas_add_component( TgcOverlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonDigitContainer MuonOverlayBase AthenaKernel StoreGateLib SGtests CxxUtils GeneratorObjects MuonIdHelpersLib MuonSimData )

# Install files from the package:
atlas_install_headers( TgcOverlay )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

