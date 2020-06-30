################################################################################
# Package: MuonRPC_CnvTools
################################################################################

# Declare the package name:
atlas_subdir( MuonRPC_CnvTools )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write DataWriter )

atlas_add_library( MuonRPC_CnvToolsLib
                   INTERFACE
                   PUBLIC_HEADERS MuonRPC_CnvTools
                   LINK_LIBRARIES GaudiKernel ByteStreamData )

# Component(s) in the package:
atlas_add_component( MuonRPC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} MuonRPC_CnvToolsLib ByteStreamData_test GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib SGtests ByteStreamCnvSvcBaseLib MuonCondInterface MuonCondData MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MuonTrigCoinData TrkSurfaces TrigT1RPChardwareLib RPC_CondCablingLib RPCcablingInterfaceLib MuonCnvToolInterfacesLib xAODEventInfo )

