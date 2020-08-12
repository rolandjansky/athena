# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( OverlayByteStreamUtils )

# External dependencies:
find_package( ROOT COMPONENTS MathCore )

# Component(s) in the package:
atlas_add_component( OverlayByteStreamUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel ByteStreamCnvSvcLib GaudiKernel StoreGateLib TrigConfHLTData TrigConfInterfaces TrigSteeringEvent TrigT1Result )
