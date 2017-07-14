/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkVertexOnTrack_page The VertexOnTrack class
@author haertel@mppmu.mpg.de

@section TrkVertexOnTrack_introductionTrkVertexOnTrack Introduction

Class to handle vertices as measurements in fitters and on track objects, it inherits from the common MeasurementBase.
Typically a VertexFinder is run to get the best possible VertexCandidates. A VertexOnTrack instance then needs to be
created for every track assoziated to a give VertexCandidate. Together with the hits from a track the VertexOnTrack 
can then be used to fit a new track which has the vertex as constraining measurement.

This mechanism is e.g. used by the InnerDetector alignment approaches as one way to get vertexing information into
the alignment procedures.



@namespace Trk
A namespace to enclose the Tracking classes.
*/
