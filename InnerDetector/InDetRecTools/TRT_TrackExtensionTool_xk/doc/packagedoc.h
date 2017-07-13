/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_TrackExtensionTool_xk_page TRT_TrackExtensionTool_xk
@author Igor.Gavrilenko@cern.ch
@section TRT_TrackExtensionTool_xk_TRT_TrackExtensionTool_xkIntro Introduction

This tool is concrete implementation for ITRT_TrackExtensionTool
and is devoted for vector<const Trk::MeasurementBase*>& production using Trk::TrackParameters& after
track finding in Pixel and SCT.

This TRT track extention algorithm contains next steps.

1. Track parameters at the outermost 'precision' SCT detector element, i.e. thst closest to the TRT together 
   with ITRT_DetElementsRoadMake tool  are used to define a narrow road through the TRT. The widht of this 
   road is defined typically as 10-20 mm.

2. Detector elements from this road are used as input for internal trajectory production. Each element of the trajectory
   contains information about trajectory position, TRT detector element and all its TRT active drift circles.
   All drift cicles inside this road with their drift-time infomation, are used as input for local pattern recognition
   in the TRT. 

3. The algorithm calculate new position each drift circle in system coordinate of the trajectory. It give posibility
   present trajectory as straight line. The x-axis of this trsaight line is then chosen to be parralel 
   to the trajectory direction and each straw drift circle can be represented in y-direction as three segment normal 
   to the x-axis. One segment represents the complete straw and the other two reperesent the possible track coordinates
   corresponding to the twofold ambiguity of the drift-time information in the straws.      
   
4. The best track prolongation through the TRT can be defined as the straight line in x-y plane (y = A*x+B),
   which crosses the largest number of straw segments. The local pattern recognition through the TRT use unbinned
   histogramming method. Each straw segment is present as two boundary hits on y-axis, with weight -1 for the upper
   and +1 for the lower one. The straight line seach in the x-y plane is peformed as a recursive algorithm over a
   set of track rotation arround a given boundary hit, Each step of this algorithm, starting e.g. from some
   arbitrary boundary hit i consist of three sequential operation :

    a. calculation of slope Aij and Wij for each i,j pair;
    b. sorting of all (Aij,Wij) in ascending order of A;
    c. identification of the pair of adjacent boundary hits(j1,j2) which maximise the total weight Wij.
   
   The new boundary hit j1 or j2 are used instead i for the next step in the iteration and the algorithm stops
   the search if both boundary hits j1 and j2 are already flagged as having been used in a previous step.
   The procedure outputs the parameters of the best straight line trajectory through the TRT and all straws
   hits and drift-time hits intercepted by this line are used for Trk::MeasurementBase production using
   IRIO_OnTrackCreator tool.
     
This algorithm uses IMagneticFieldTool, IPropagator, ITRT_DetElementsRoadMaker, IRIO_OnTrackCreator tools.


*/
