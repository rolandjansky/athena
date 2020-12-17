# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigPrepRawDataFormat )

# Component(s) in the package:
atlas_add_component( InDetTrigPrepRawDataFormat
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaKernel AtlasDetDescr ByteStreamCnvSvcBaseLib EventContainers GaudiKernel IRegionSelector Identifier InDetByteStreamErrors InDetConditionsSummaryService InDetIdentifier InDetPrepRawData InDetRawData InDetReadoutGeometry InDetTrigToolInterfacesLib PixelConditionsData SCT_ConditionsData SiClusterizationToolLib StoreGateLib TRT_ConditionsServicesLib TRT_DriftCircleToolLib TRT_ReadoutGeometry TrigInterfacesLib TrigSteeringEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
