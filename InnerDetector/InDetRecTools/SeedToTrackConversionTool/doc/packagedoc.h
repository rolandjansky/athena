/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SeedToTrackConversionTool_page SeedToTrackConversionTool Package
@author Weiming Yao <Weiming.Yao@CERN.CH>

@section SeedToTrackConversionTool_SeedToTrackConversionToolIntro Introduction

  This is a tool to convert the track seeds as the track collection and saved as SiSPSeedSegments objects in xAOD.
  SiTrackMaker_xk first performs estimation of initial track parameter of seeds using three spacepoints and searches
  hits inside a road to identify possible set of track candidates. If the track is found with the seeds, the seeds 
  would be called survived seeds. What the tool does is to write out the seeds after passing some basic 
  requirement and the name of pass and survive information are also set by 
  setPatternRecognitionInfo and setTrackFitter.   
  What is required: configure the tool in ConfiguredNewTrackingSiPattern.py, similar to configure SiTrackMaker.
  Obtain the tool in SiTrackMakerTool package and do the following:
   m_seedtotrackconversion->newEvent(); initialize for each event
   m_seedtotrackconversion->executeSiSPSeedSegments(Tp, m_tracks.size(), Sp); for each seeds 
   m_seedtotrackconversion->endEvent(); save SiSPSeedSegment objects 
 That's it and send in your feedbacks. 



*/
