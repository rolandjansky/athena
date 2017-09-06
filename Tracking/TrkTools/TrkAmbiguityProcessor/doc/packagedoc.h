/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrkAmbiguityProcessor_page 

The TrkAmbiguityProcessor is responsible for resolving ambiguities in the passed TrackCollection. To do this, it attempts to improve the 'score' of an event, where the score of an event is the summed scores of all the tracks it contains. To this end, there is a (currently simplistic) Trk::TrackScoringTool, as well as a Trk::ITrackScoringHelperTool interface (to allow for ID and muon-specific implementation, without introducing a dependency). Additionally, the TrkAmbiguityProcessor makes use of a Trk::PRD_AssociationTool which keeps track of all the Trk::PrepRawData on the tracks, and knows which are shared with which Trk::Track etc.

There are two algtools provided:
   - SimpleAmbiguityProcessorTool
   - TrackSelectionProcessorTool
   
For more details see "A New Algorithm For Solving Tracking Ambiguities, Daniel Wicke"



*/
