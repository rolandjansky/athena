################################################################################
# Package: MuonCablingData
################################################################################

# Declare the package name:
atlas_subdir( MuonCablingData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          MuonSpectrometer/MuonIdHelpers )

# Component(s) in the package:
atlas_add_library( MuonCablingData
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingData
                   LINK_LIBRARIES SGTools GaudiKernel StoreGateLib SGtests MuonIdHelpersLib )

