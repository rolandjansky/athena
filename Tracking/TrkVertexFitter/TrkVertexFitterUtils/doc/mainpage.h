/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  TrkVertexFitterUtils

@author Kirill.Prokofiev@cern.ch
\section introTrkVertexFitterUtils Introduction
This package contains a set of tools required for
various vertex fitting applications. Most of theses
tools are descendants of abstract interfaces defined
in TrkVertexFitterInterfaces package. The tools are
completely independent and can be used together or
separately from each other. For a brief description of
tools see below. For examples of use of these utils
see <i>TrkVertexFitters</i> package.

\section designTrkVertexFitterUtls Design 
<b><i>Trk::Chi2TrackCompatibilityEstimator</i></b> A concrete 
implementation of Trk::IVertexTrackCompatibilityEstimator.
Estimates the compatibility a track to a vertex candidate
using a chi2 method: constructs the chi2 using a distance
of a closest approach and a covariance matrix of a track. 
<br><br>
<b><i> Trk::DetAnnealingMaker</i></b> A concrete 
implementation of Trk::IVertexAnnealingMaker. Represents 
an annealing procedure to be applied in adaptive fits.
The actual annealing schedule (a set of temperatures)
is a property of this tool and can be set up through the 
jobOptions.
<br><br>
<b><i> Trk::DummyAnnealingMaker</i></b> A dummy implementation
of Trk::IVertexAnnealingMaker. The reasons for this class existence 
are purely technical, moreover it may quite possibly disappear at 
later stages. 
<br><br>
<b><i> Trk::FullLinearizedTrackFactory</i></b> A helper class, 
used to calculated the parameters of expansion of the 
measurement equation at the point of closest approach. 
The result is then stored in Trk::LinearizedTrack object and 
returned to the user (typically vertex fitter).
<br><br>
<b><i> Trk::ImpactPoint3dAtaPlaneFactory</i></b> A helper 
class creating a parameters of track at 3D point of closest
approach to a given point (e.g. vertex).
<br><br>
<b><i> Trk::ImpactPoint3dEstimator </i></b>A helper class 
finding a 3D point of closest approach of a track to the
given reference point (e.g. vertex).
<br><br>
<b><i> Trk::KalmanVertexTrackUpdator</i></b> A concrete implementation of
a Trk::IVertexSmoother. A Kalman smoother for tracks fitted to a vertex. 
Constrains a provided track to pass exactly through the position of a 
reconstructed vertex. Thus the state of the track is updated with the 
knowledge of its point of origin.
<br><br>
<b><i> Trk::KalmanVertexUpdator </i></b> A concrete implementation of
a Trk::IVertexUpdator. A Kalman updator for vertexing. Adds or removes 
a single track to the vertex candidate, calculates the new position and 
covariance matrix of  the vertex.

@htmlinclude used_packages.html
@include requirements

*/
