# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_CalibTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS RIO Core Tree MathCore Hist Graf Gpad )

atlas_add_library( TRT_CalibToolsLib
                   TRT_CalibTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_CalibTools
                   LINK_LIBRARIES CxxUtils GaudiKernel TRT_CalibData Identifier xAODEventInfo xAODTracking )

# Component(s) in the package:
atlas_add_component( TRT_CalibTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AtlasDetDescr CommissionEvent CxxUtils EventPrimitives GaudiKernel Identifier InDetIdentifier InDetPrepRawData InDetRIO_OnTrack TRT_CalibData TRT_CalibToolsLib TRT_ConditionsData TRT_ConditionsServicesLib TRT_DriftCircleToolLib TRT_DriftFunctionToolLib TRT_ReadoutGeometry TrkEventPrimitives TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkToolInterfaces TrkTrack TrkTrackSummary xAODEventInfo xAODTracking )
