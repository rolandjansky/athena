################################################################################
# Package: MuonCablingData
################################################################################

# Declare the package name:
atlas_subdir( MuonCablingData )

# Component(s) in the package:
atlas_add_library( MuonCablingData
                   src/*.cxx
                   PUBLIC_HEADERS MuonCablingData
                   LINK_LIBRARIES AthenaKernel GaudiKernel StoreGateLib SGtests MuonIdHelpersLib )

