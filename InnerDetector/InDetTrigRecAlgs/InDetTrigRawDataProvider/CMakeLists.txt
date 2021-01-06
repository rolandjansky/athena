# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrigRawDataProvider )

# Component(s) in the package:
atlas_add_component( InDetTrigRawDataProvider
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRawData AthenaKernel IRegionSelector ByteStreamCnvSvcBaseLib InDetByteStreamErrors InDetIdentifier SCT_CablingLib TrigSteeringEvent InDetTrigToolInterfacesLib SCT_RawDataByteStreamCnvLib PixelRawDataByteStreamCnvLib TRT_ConditionsServicesLib TRT_CablingLib TRT_RawDataByteStreamCnvLib )
