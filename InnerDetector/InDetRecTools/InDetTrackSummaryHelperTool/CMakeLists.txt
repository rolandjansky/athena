# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrackSummaryHelperTool )

# Component(s) in the package:
atlas_add_component( InDetTrackSummaryHelperTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib TrkEventPrimitives TrkTrackSummary TrkToolInterfaces Identifier InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces TrkCompetingRIOsOnTrack TrkParameters TrkRIO_OnTrack TrkTrack TrkEventUtils)
