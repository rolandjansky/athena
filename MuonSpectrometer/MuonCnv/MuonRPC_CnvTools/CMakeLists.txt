################################################################################
# Package: MuonRPC_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonRPC_CnvTools )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

# Component(s) in the package:
atlas_add_component( MuonRPC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests ByteStreamCnvSvcBaseLib MuonCondInterface MuonCondData MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MuonTrigCoinData TrkSurfaces TrigT1RPChardwareLib RPChardware RPC_CondCablingLib RPCcablingInterfaceLib MuonCnvToolInterfacesLib )

# Install files from the package:
atlas_install_headers( MuonRPC_CnvTools )

