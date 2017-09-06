/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
@page TrkVertexFitterInterfaces_page The TrkVertexFitterInterfaces package

@author Kirill.Prokofiev@cern.ch

@section TrkVertexFitterInterfaces_introductionTrkVertexFitterInterfaces Introduction

This package  contains abstract interfaces for the ATLAS 
vertex fitters and related classes, such as VertexUpdators, 
SeedFinders, TrackToVertexCompatibilityEstimators VertexSmoothers
and AnnealingMakers. These abstract interface classes are 
common for all ATLAS vertexing-related implementations. 
The design is supposed to handle both iterative(Kalman filter-based)
and global(global chi2-based) vertex fitters. The extensions of this
interface are also used for a constraint fits (see V0). 

@section TrkVertexFitterInterfaces_designTrkVertexFitterInterfaces Design
The design of the base classes is the following:
<br>
<b><i>Trk::IVertexFitter</i></b>
Top level interface for ATLAS vertex fitters.
Defines that any vertex fitter should return a 
<i>Trk::VxCandidate</i>, given with a set of tracks of 
their parameters (<i>TrackParticleBase</i> application is also
planned)
<br>
<b><i>Trk::IVertexUpdator</i></b>
Defines the interface for vertex updators to be used in iterative fitters.
A vertex updator adds or removes a single track to or from a given <i>VxCandidate</i>
<br>
<b><i>Trk::IVertexSmoother</i></b>
Defines an interface for vertex smoothers. A vertex smoother refits the track
contents of a reconstructed vertex with a knowledge of its reconstructed position.
In fact, the tracks are forced to pass through the reconstructed vertex, which 
becomes their common space point. Smoother  top user method is void: it updates 
an internal structure if the <i>Trk::VxCandidate</i>.
<br>
<b><i>Trk::IVertexTrackUpdator</i></b>
A vertex track updator smooths one track at the time. Normally, called iteratively
by a vertex smoother.
<br>
<b><i>Trk::IVertexTrackCompatibilityEstimator </i></b>
Interface for classes, estimating track compatibility to the vertex. Implementation
can be  a chi2 for instance. The void method also supposed to update the chi2 data member
of <i>Trk::VxTrackAtVertex</i>
<br>
<b><i>Trk::IVertexSeedFinder</i></b>
Interface for vertex seed finders. This group of classes 
estimates the starting point of the vertex fit. A <i>Vertex</i>
is object is returned given a set tracks.
<br>
<b><i>Trk::IVertexAnnealingMaker</i></b>
Interface for classes responsible for annealing process.
Used in adaptive vertex fits.
<br>
<b><i>Trk::IVertexLinearizedTrackFactory</i></b>
An abstract interface for helper classes, calculating
parameters of Taylor expansion of a measurement equation.
For internal use of iterative vertex fitters.
<br>
<b><i>Trk::IVertexKinematicFitter</i></b>
Interface for kinematic vertex-fitting.
<br>
<b><i>Trk::IKinematicConstraint</i></b>
Abstract class used for implementing constraint-equations used
by the kinematic vertex-fitting methods.


*/
