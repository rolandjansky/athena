# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_TrackHoleSearch )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree )

atlas_add_library( TRT_TrackHoleSearchLib
                   TRT_TrackHoleSearch/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_TrackHoleSearch
                   LINK_LIBRARIES GaudiKernel AthenaBaseComps TrkToolInterfaces TrigDecisionInterface TRT_ConditionsServicesLib TrkTrack xAODEventInfo xAODTracking StoreGateLib TrkParameters TrkExInterfaces InDetConditionsSummaryService )

# Component(s) in the package:
atlas_add_component( TRT_TrackHoleSearch
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} EventPrimitives GaudiKernel Identifier InDetConditionsSummaryService InDetIdentifier InDetRIO_OnTrack StoreGateLib TRT_ConditionsServicesLib TRT_TrackHoleSearchLib TrkEventPrimitives TrkEventUtils TrkExInterfaces TrkParameters TrkSurfaces TrkTrack TrkTrackSummary )

# Install files from the package:
atlas_install_joboptions( share/*.py )

