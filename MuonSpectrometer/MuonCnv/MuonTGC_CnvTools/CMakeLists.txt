# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonTGC_CnvTools )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat )

atlas_add_library( MuonTGC_CnvToolsLib
                   MuonTGC_CnvTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonTGC_CnvTools
                   LINK_LIBRARIES GaudiKernel ByteStreamData )

# Component(s) in the package:
atlas_add_component( MuonTGC_CnvTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GaudiKernel AthenaBaseComps Identifier FourMomUtils
                                                              EventPrimitives TGCcablingInterfaceLib MuonReadoutGeometry MuonDigitContainer MuonIdHelpersLib MuonRDO 
                                                              MuonPrepRawData MuonTrigCoinData TrkSurfaces MuonCnvToolInterfacesLib MuonTGC_CnvToolsLib ByteStreamCnvSvcLib CxxUtils )

# Install files from the package:
atlas_install_joboptions( share/*.py )

