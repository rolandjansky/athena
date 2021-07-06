# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTResultByteStream )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat_write )

# Component(s) in the package:
atlas_add_component( TrigHLTResultByteStream
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamCnvSvcBaseLib ByteStreamData GaudiKernel StoreGateLib TrigSteeringEvent )
