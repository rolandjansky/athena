/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetZVTOPVxFinder_page InDetZVTOPVxFinder Package

Contains:
- ZVTOP_AmbiguitySolver.cxx (dummy) <br>
- ZVTOP_SimpleVtxProbCalc.cxx (helper class, calculates vtx probability at a given point) <br>
- ZVTOP_SpatialPointFinder.cxx (helper class, seed finder for vertex candidates) <br>
- ZVTOP_Tool.cxx (main tool) <br>
- ZVTOP_TrkProbTubeCalc.cxx (helper class, calculates track probabilities at a given point) <br>
- ZVTOP_VertexState.cxx (helper class) <br>

Documentation: David J. Jackson, Nucl.Instrum.Meth.A388: 247-253, 1997

@author Tatjana Lenz <tatjana.lenz@cern.ch>

@section InDetZVTOPVxFinder_InDetZVTOPVxFinderIntro Introduction

This AlgTool reconstructs a set of vertices and associated subset of the charged tracks, the association accords to a vertex probability function which is based on the trajectories and position resolution of the tracks. The track probability function, \f$ f_i \f$ is defined as a Gaussian tube around a track: the width is given by a track covariance (= error) matrix. The vertex probability function is \f$ \sum f_i - \frac{ \sum f_i^2}{\sum f_i} \f$. The job is to search for local maxima of the vertex probability function. So you have to calculate the vertex probability function at euch space point in detector. To reduce the number of spatial points, all possible combinations of two track pairs are build. For each pair the spatial point is calculated (see ZVTOP_SpatialPointFinder), all spatial points are candidates for a vertex. To reduce the number of vertex candidates the vertex candidate cluster are build. The tracks associated with each cluster are fitted, the vertex fitter can be set/changed via jobOptions, default Billoir FullFitter.
On the fitted VxCandidate some quality cuts can be applied. The result is stored in a VxContainer. 



*/
