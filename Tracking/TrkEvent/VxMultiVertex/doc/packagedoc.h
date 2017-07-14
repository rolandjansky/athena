/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page VxMultiVertex_page VxMultiVertex Package

@author Kirill.Prokofiev@cern.ch

@section VxMultiVertex_introVxMultiVertex Introduction
This package is designed to provide an 
Event Data Model for a vertex reconstruction performed with
multi vertex adaptive technique.
The Trk::MVFVxCandidate and Trk::MVFVxTrackAtVertex classes 
inherit form the corresponding classes of VxVertex library.
Presently, all the additional data members are made transient. 
In the case the storage of these members will be required in 
future, a special MVFVxContainer is provided.

@section VxMultiVertex_designVxMultiVertex Design
<b><i>Trk::MVFVxCandidate </i></b>  An extention of 
<i>Trk::VxCandidate</i>. This class uses <i>Trk::MVFVxTrackAtVertex</i>
to store the information about tracks participating in the 
vertex fit and <i>Trk::MvfFitInfo</i> to keep track of fit initial
setup.
<br><br>
<b><i>Trk::MVFVxTrackAtVertex</i></b> An extension of <i>Trk::VxTrackAtVertex</i>.
In addition, it remembers, which vertices this track was assocaited with.
<br><br>
<b><i>Trk::TrackToVtxLink</i></b> Keeps the link od a given <i>Trk::Track</i> of 
<i>Rec::TrackParticle</i> to the assocaited vertices.
<br><br>
<b><i>Trk::MvfFitInfo</i></b> A helper storage class for the information required during 
an adaptive multi vertex fit.
<br><br>
<b><i>MVFVxContainer</i></b>  Top level object of the package. A collection
of <i>Trk::MVFVxCandidate </i>. It is not used so far, but might become 
quite important if any of presently transient MVF-related stuff becomes
persistent.

*/
