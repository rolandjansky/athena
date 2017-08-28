/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkVertexFitterValidationUtils_page TrkVertexFitterValidationUtils Package

@author Kirill.Prokofiev@cern.ch

@section TrkVertexFitterValidationUtils_introTrkVertexFitterValidationUtils Introduction
This package is designed to contain a set of helper 
utils suitable for the estimation of efficiency of   
primary vertex reconstruction.
@section TrkVertexFitterValidationUtils_designTrkVertexFitterValidationUtils Design

<b><i>Trk::TrkPriVxPurity </i></b> 
A storage object designed
to save the results of the purity analysy of reconstructed primary
vertex. 

<br><br>
<b><i>Trk::TrkPriVxPurityTool</i></b> 
A tool retrieveing all 
the  purity information about the primary vertex candidate:
number of correct tracks and their weights, number of
pileup tracks and their weight etc.. The outlyer tracks are 
defined as those coming from vertexes in the configurable region
around  a primary collision in Monte Carlo.

*/
