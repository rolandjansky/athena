################################################################################
# Package: MuonRdoToPrepData
################################################################################

# Declare the package name:
atlas_subdir( MuonRdoToPrepData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          PRIVATE
                          Control/StoreGate
                          Control/AthViews
                          DetectorDescription/Identifier
                          Trigger/TrigSteer/ViewAlgs
			                    Trigger/TrigEvent/TrigSteeringEvent
			                    DetectorDescription/IRegionSelector 
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonTrigCoinData)

# Component(s) in the package:
atlas_add_component( MuonRdoToPrepData
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib SGtests Identifier TrigSteeringEvent IRegionSelector MuonPrepRawData MuonTrigCoinData AthViews MuonCnvToolInterfacesLib ViewAlgsLib )

# Install files from the package:
atlas_install_headers( MuonRdoToPrepData )

