# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_RawDataByteStreamCnv )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )

atlas_add_library( TRT_RawDataByteStreamCnvLib
                   TRT_RawDataByteStreamCnv/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_RawDataByteStreamCnv
                   LINK_LIBRARIES AthenaBaseComps ByteStreamData InDetRawData Identifier )

# Component(s) in the package:
atlas_add_component( TRT_RawDataByteStreamCnv
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities ByteStreamCnvSvcBaseLib ByteStreamCnvSvcLib ByteStreamData CxxUtils GaudiKernel IRegionSelector Identifier InDetByteStreamErrors InDetIdentifier InDetRawData PathResolver StoreGateLib TRT_CablingLib TRT_RawDataByteStreamCnvLib TRT_ReadoutGeometry TrigSteeringEvent )

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
