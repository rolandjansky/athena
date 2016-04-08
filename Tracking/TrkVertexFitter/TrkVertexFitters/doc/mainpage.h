/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  TrkVertexFitters

@author Nicola.Giacinto.Piacquadio@cern.ch

\section introTrkVertexFitters Introduction
This package contains the Vertex Fitters in the 
form of Athena AlgoTools, which are needed for 
vertex finding applications. The input is typically 
a bunch of Tracks or TrackParameters, eventually an 
initial position and a constraint vertex, and the output 
the fitted vertex.
For a description of the finders which uses these tools, 
please give a look at the <i>InDetPriVxFinderTool</i> package.

\section designTrkVertexFitters Design 

<li><b><i>Trk::SequentialVertexFitter</i></b></li>

An iterative vertex fitter, updating the vertex estimate with
a single track at a time. The measurement equation is relinearized
with the position of the resulting estimate and the vertex is refit. 
The iterations are performed until the vertex position improves or
 the maximal number of iterations is reached.
Smoothing (refit of tracks with the knowledge of the vertex position)
 is then performed on request. 
The following properties can be set per jobOptions:
- MaxIterations 
- MaxShift 
(in mm).

<li><b><i>Trk::AdaptiveVertexFitter</i></b></li>

This vertex algorithm is also based on the typical Kalman-Filtering
approach, with the difference that the inverse of the covariance matrices
of the track parameters of the tracks which enter the fit
are reweighted according to the probability of the tracks to be compatible
with the vertex to be fit. In this way outliers/tracks which do not belong to
the vertex are smoothly rejected.

Since the estimation of the vertex position during the first fit iterations is
not particularly good and the compatiblity estimation of the tracks not always
reliable, an annealing technique is also implemented, in order to make the rejection
of outliers soft in the beginning and harder towards the end of the fit
(for this the Annealing Maker class is responsible: as a default the Temperature of this
thermodinamical procedure is lowered according to few predefined steps)

For greater modularity the algorithm is divided into 6 modules (please look at 
the <b><i>TrkVertexFitterUtils</i></b> package for these):
- 1. SeedFinder
- 2. LinearizedTrackFactory
- 3. TrackCompatibilityEstimator
- 3b. ImpactPoint3dEstimator
- 4. VertexUpdator
- 5. VertexSmoother
- plus another one, which is responsible for letting the temperature go down
- (the so called "annealing technique")
- 6. AnnealingMaker

This kind of algorithm was implemented for the first time by CMS:
for information about the theoretical motivations a good
reference is Wolfgang Waltenberger's PhD Thesis  and many of the
calculations implemented have been extensively shown in
theoretical & experimental papers by Fruehwirth, Waltenberger, Strandlie et al.

The following parameters can be set:
- MaxIterations (limit on fit iterations)
- MaxDistToLinPoint (if the distance from the point where the track was 
linearized to the new fitted vertex is higher than this number in mm, then 
the track will be relinearized before the next iteration)
- InitialError (initial value of the diagonal weight matrix of the vertex before fit,
 so the name is really bad, because you have to provide the inverse of the error squared. Should 
be small enough not to result in biases, but still such that it doesn't create numerical problems)
- onlyzseed (requests seeding the primary vertex only along the z direction, for x and y the value of 
the beam spot vertex is used)
- DoSmoothing (if true enables smoothing)

<li><b><i> Trk::AdaptiveMultiVertexFitter</i></b></li>

This class implements a multi vertex fitting algorithm
which is adaptive and robust, quite insensitive to the presence
of outliers.

The multi-vertex version of the fit is more complicate than the
single-vertex version and needs the input to be provided in the form
of a vector of already initialized MVFVxCandidate objects. Details of the
the objects to provide you find in the fit() and addVtxTofit() functions.

During the multi-vertex fit all the vertices are fit, using the tracks
which are contained in their respective vertex candidates. Tracks which are
shared among more than one vertex get their weights adjusted dynamically
iteration after iteration thanks to a Deterministic Annealing
procedure, so that in the end they are dinamically assigned to the
vertex they most probably belong to.

Parameters to be set per joboptions:
- MaxIterations
- MaxDistToLinPoint
- InitialError
- DoSmoothing
(for these the meaning is the same as for the single-vertex 
AdaptiveVertexFitter)
- MinWeight (all the tracks which during the fit acquire a value 
which is lower than this predefined value, are not considered anymore in the fit)

<li><b><i> Trk::DummyVertexSmoother</i></b></li>

The vertex smoother 
is used by all the fitter as a final step to improve the Track Parameters 
of the Tracks with the knowledge of the fitted vertex. 
The Dummy Vertex Smoother however does not refit trajectories 
with the knowledge of the vertex. Instead it takes
trajectories, extrapolate them to the transverse point 
of the closest approach to the reconstructed vertex
and calculates the perigee parameters of trajectories
with respect to that vertex.

<li><b><i> Trk::SequentialVertexSmoother</i></b></li>

A concrete implementation of 
the Vertex smoother. Takes the tracks from the  VxCandidates and updates 
them with the knowledge of the reconstructed vertex 
position. Can be used with any VertexTrackUpdator.
The default is KalmanVertexTrackUpdator

@htmlinclude used_packages.html
@include requirements

*/
