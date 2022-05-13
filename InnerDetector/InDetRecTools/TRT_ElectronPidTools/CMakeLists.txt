# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_ElectronPidTools )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core MathCore )

atlas_add_library( TRT_ElectronPidToolsLib
                   TRT_ElectronPidTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_ElectronPidTools
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaKernel TrkTrack InDetRawData TrkParameters TrkToolInterfaces TRT_ConditionsData TrkEventPrimitives TRT_ConditionsServicesLib InDetPrepRawData StoreGateLib TRT_ConditionsNNLib )
# Component(s) in the package:
atlas_add_component( TRT_ElectronPidTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaKernel AthenaPoolUtilities GaudiKernel Identifier InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRawData StoreGateLib TRT_ElectronPidToolsLib TRT_ReadoutGeometry TrkMeasurementBase TrkParameters TrkRIO_OnTrack TrkSurfaces TrkTrack TrkTrackSummary xAODEventInfo )
