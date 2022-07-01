# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonSTGC_CnvTools )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat )

atlas_add_library( MuonSTGC_CnvToolsLib
                   MuonSTGC_CnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSTGC_CnvTools
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( MuonSTGC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps Identifier EventPrimitives MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonNSWCommonDecode MuonRDO MuonPrepRawData MuonTrigCoinData TrkSurfaces STgcClusterizationLib NSWCalibToolsLib MuonCnvToolInterfacesLib MuonSTGC_CnvToolsLib )
