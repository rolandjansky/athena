/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page VxVertex_page VxVertex


@author Andreas.Wildauer@cern.ch
@author Kirill.Prokofiev@cern.ch

@section VxVertex_introVxVertex Introduction
The <i>VxVertex</i> package provides the Event Data Model for
reconstruction of primary and secondary vertexes. 
The extensions of VxVertex are also suitable  to 
store the information about constrained vertices's, V0's,
conversions etc. The top level object of the package is a
<i>Trk::VxContainer</i>, which is returned e.g. by a primary vertex 
finder and represents a collection of <i>Trk::VxCandidate</i>.
The latter one is the central object of the package, representing 
a reconstructed vertex candidate. It provides an information 
about a position of reconstructed vertex, covariance matrix  and
its track contents. In some case (e.g. constraint fitting) the 
full covariance matrix of the fit is also stored (see <i>
Trk::ExtendedVxCandidate</i>).

@section VxVertex_designVxVertex Design
<b><i>Trk::Vertex</i></b>: A simplest vertex representation. Stores a 
3d position only.
<br><br>
<b><i>Trk::RecVertex</i></b>: Inherits from the <i>Trk::Vertex</i> class. 
Stores a 3x3 covariance matrix and values related to the quality of
the fit in addition.
<br><br>
<b><i>Trk::VxCandidate</i></b>: A reconstructed vertex candidate. Includes
a <i>Trk::RecVertex</i> as a private data member. Also stores a collection of 
<i>Trk::VxTrackAtVertex</i>, thus providing information about tracks
used in the fit and their initial and refitted states.
<br><br>
<b><i>Trk::VxTrackAtVertex</i></b>: Represents the tracks used in the 
vertex fit. Contains links to initial track parameters, reconstructed track
(if any), refitted state of the track (if any). Also provides various measures
of track compatibility to the fitted vertex, such as Chi2, weight etc. The 
actual use depends strongly on the concrete fitter.
<br><br>
<b><i>Trk::ExtendedVxCandidate</i></b>: Extends the <i>Trk::VxCandidate</i>
with a full covariance matrix of the fit. The latter one is supposed to 
include all track-track, track-vertex and vertex-vertex inter component
correlations.
<br><br>
<b><i>Trk::LinearizedTrack</i></b>: A class providing parameters of the
first order Taylor expansion of the measurement equation. This is a 
storage place for various parameters required during the iterative 
vertex fit.
<br><br>
<b><i>VxContainer</i></b>: A top level object of this package. This is
a container of vertexes to be written to the StoreGate as a result of
a vertex reconstruction.

*/
