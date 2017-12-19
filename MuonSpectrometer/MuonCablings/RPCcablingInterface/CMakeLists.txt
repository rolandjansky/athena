################################################################################
# Package: RPCcablingInterface
################################################################################

# Declare the package name:
atlas_subdir( RPCcablingInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonCablings/MuonCablingTools
                          MuonSpectrometer/MuonIdHelpers
                          PRIVATE
                          Control/StoreGate )

# Component(s) in the package:
atlas_add_library( RPCcablingInterfaceLib
                   src/*.cxx
                   PUBLIC_HEADERS RPCcablingInterface
                   LINK_LIBRARIES AthenaKernel Identifier GaudiKernel MuonCablingTools CablingTools MuonIdHelpersLib StoreGateLib SGtests )

# Install files from the package:
atlas_install_python_modules( python/__init__.py )

