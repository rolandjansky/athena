################################################################################
# Package: MuonRdoToPrepData
################################################################################

# Declare the package name:
atlas_subdir( MuonRdoToPrepData )

# Component(s) in the package:
atlas_add_component( MuonRdoToPrepData
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib SGtests Identifier TrigSteeringEvent IRegionSelector MuonPrepRawData MuonTrigCoinData AthViews MuonCnvToolInterfacesLib ViewAlgsLib )

# Install files from the package:
atlas_install_headers( MuonRdoToPrepData )

