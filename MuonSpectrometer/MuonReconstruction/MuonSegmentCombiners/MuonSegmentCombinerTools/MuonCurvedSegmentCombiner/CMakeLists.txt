# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonCurvedSegmentCombiner )

# Component(s) in the package:
atlas_add_component( MuonCurvedSegmentCombiner
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonEDM_AssociationObjects MuonReadoutGeometry MuonIdHelpersLib MuonPattern MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib TrkCompetingRIOsOnTrack TrkEventPrimitives TrkParameters TrkPrepRawData TrkRIO_OnTrack MuonSegmentCombinerToolInterfaces )
