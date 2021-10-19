# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelRawDataByteStreamCnv )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat )

# Component(s) in the package:
atlas_add_library( PixelRawDataByteStreamCnvLib
   PixelRawDataByteStreamCnv/*.h
   INTERFACE
   PUBLIC_HEADERS PixelRawDataByteStreamCnv
   LINK_LIBRARIES GaudiKernel ByteStreamData InDetByteStreamErrors InDetRawData )

atlas_add_component( PixelRawDataByteStreamCnv
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} ByteStreamData AthenaKernel EventContainers
   GaudiKernel InDetRawData AthenaBaseComps AthContainers CxxUtils StoreGateLib
   ByteStreamCnvSvcBaseLib InDetIdentifier PixelReadoutGeometryLib IRegionSelector
   xAODEventInfo TrigSteeringEvent InDetByteStreamErrors PixelConditionsData PixelRawDataByteStreamCnvLib ByteStreamCnvSvcLib )

   atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
