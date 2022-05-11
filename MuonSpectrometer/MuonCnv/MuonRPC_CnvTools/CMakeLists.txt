# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonRPC_CnvTools )

# External dependencies:
find_package( tdaq-common )

atlas_add_library( MuonRPC_CnvToolsLib
                   INTERFACE
                   PUBLIC_HEADERS MuonRPC_CnvTools
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} GaudiKernel ByteStreamData MuonCnvToolInterfacesLib )

# Component(s) in the package:
atlas_add_component( MuonRPC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} MuonRPC_CnvToolsLib ByteStreamData_test GaudiKernel AthenaBaseComps AthenaKernel CxxUtils StoreGateLib ByteStreamCnvSvcBaseLib MuonCondInterface MuonCondData MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData MuonTrigCoinData TrkSurfaces TrigT1RPChardwareLib RPC_CondCablingLib MuonCnvToolInterfacesLib xAODEventInfo ByteStreamCnvSvcLib )
