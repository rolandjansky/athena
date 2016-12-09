################################################################################
# Package: RpcOverlay
################################################################################

# Declare the package name:
atlas_subdir( RpcOverlay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonDigitContainer
                          MuonSpectrometer/MuonOverlay/MuonOverlayBase
                          PRIVATE
			  Control/CxxUtils  	  
                          Control/StoreGate
                          Generators/GeneratorObjects
                          MuonSpectrometer/MuonDigitization/MuonDigToolInterfaces
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonSimData )

# Component(s) in the package:
atlas_add_component( RpcOverlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel MuonDigitContainer MuonOverlayBase StoreGateLib SGtests CxxUtils GeneratorObjects MuonIdHelpersLib MuonSimData )

# Install files from the package:
atlas_install_headers( RpcOverlay )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

