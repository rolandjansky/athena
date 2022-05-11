# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonAmbiTrackSelectionTool )

# Component(s) in the package:
atlas_add_component( MuonAmbiTrackSelectionTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkToolInterfaces AthContainers MuonIdHelpersLib MuonRecHelperToolsLib TrkCompetingRIOsOnTrack TrkEventUtils TrkMeasurementBase TrkRIO_OnTrack TrkTrack )
