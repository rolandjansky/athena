# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTResultByteStream )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write )

# Component(s) in the package:
atlas_add_library( TrigHLTResultByteStreamLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigHLTResultByteStream
                   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps ByteStreamCnvSvcBaseLib ByteStreamData GaudiKernel TrigSteeringEvent
                   PRIVATE_LINK_LIBRARIES AthenaKernel StoreGateLib )

atlas_add_component( TrigHLTResultByteStream
                     src/components/*.cxx
                     LINK_LIBRARIES TrigHLTResultByteStreamLib )
