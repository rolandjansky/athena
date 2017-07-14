/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page SiTrackMakerTool_xk_page SiTrackMakerTool_xk
@author Igor.Gavrilenko@cern.ch
@section SiTrackMakerTool_xk_SiTrackMakerTool_xkIntro Introduction

This tool is concrete implementation for ISiTrackMaker
and is devoted for list<Trk::Track*>& production using list<const Trk::SpacePoint*> as input. 

This track finding algorithm contains 6 steps.

1. SiTrackMaker_xk performs estimation initial track parameters of the trajectory using initial set space points.

2. Using this track parameters and  ISiDetElementsRoadMake tool produces detector elements road sorted 
   in propagation order.

3. Detector elements road are used as input for internal trajectory production. Each element of the trajectory
   contains information about detector element and all his clusters. 

4. Using Trk::PrepRawData from trigger space points test quality initial trajectory using
   Kalman filter-smoother formalism.
   The quality of trajectory is determined from its lengh or number of clusters, number holes and its total Xi2.

5. If quality initial trajectory is good  SiTrackMaker_xk make trajectory extension both in inner and outer radii
   using Kalman filter together with combinatorial pattern recognition. Combinatorial pattern recognition include
   investigation different possible trajectory prolongations and choice best of them. 

 6. If a valid prolongations are exist SiTrackMaker_xk performs the final smoother procedure and
    convert internal trajectory to standard Trk::Track using IRIO_OnTrackCreator tool.
     
This algorithm uses IMagneticFieldTool, IPropagator, IUpdator, IRIO_OnTrackCreator, ISiDetElementsRoadMaker tools.


*/
