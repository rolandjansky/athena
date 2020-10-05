# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigRoiConversion )

# Component(s) in the package:
atlas_add_library( TrigRoiConversionLib
   TrigRoiConversion/*.h Root/*.cxx
   PUBLIC_HEADERS TrigRoiConversion
   LINK_LIBRARIES AsgTools TrigSteeringEvent
   PRIVATE_LINK_LIBRARIES xAODTrigger )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrigRoiConversion
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps TrigRoiConversionLib xAODTrigger )
endif()
