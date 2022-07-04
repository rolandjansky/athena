# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCSC_CnvTools )

# External dependencies:
find_package( CLHEP )
find_package( tdaq-common COMPONENTS eformat )

atlas_add_library( MuonCSC_CnvToolsLib
                   MuonCSC_CnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonCSC_CnvTools
                   LINK_LIBRARIES GaudiKernel ByteStreamData )

# Component(s) in the package:
atlas_add_component( MuonCSC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ${CLHEP_LIBRARIES} ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps AthenaKernel StoreGateLib Identifier ByteStreamCnvSvcBaseLib EventInfo EventPrimitives CSCcablingLib MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO MuonPrepRawData TrkSurfaces MuonCnvToolInterfacesLib CscCalibToolsLib MuonDigToolInterfacesLib MuonCSC_CnvToolsLib ByteStreamCnvSvcLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

