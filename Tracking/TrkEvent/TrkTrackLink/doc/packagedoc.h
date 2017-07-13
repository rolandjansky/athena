/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkTrackLink_page TrkTrackLink

@author Kirill.Prokofiev@cern.ch
@author Andreas.Wildauer@cern.ch

@section TrkTrackLink_PackageDescription Package Description

This package was created to resolve the circular dependency between the VxVertex and the TrkParticleBase package. The circular dependency
happened because the Trk::VxTrackAtVertex object needs to have an ElementLink to a Trk::TrackParticleBase object and the latter has an
ElementLink to a Trk::VxCandidate object. The Trk::ITrackLink class in this package replaces the ElementLink to the Track(ParticleBase)
in the Trk::VxTrackAtVertex class and resolves this circular depenency therefore.

The actual links which are used now are the classes Trk::LinkToTrack and Trk::LinkToTrackParticle which both inherit from Trk::ITrackLink
and ElementLink< TrackCollection > or ElementLink< TrackParticleBaseContainer >, respectively.


*/
