/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage InDetParticleCreation

@author Andreas.Wildauer@cern.ch

@section Introduction
The creation of Rec::TrackParticle objects is one of the last steps of the post processing chain in the Inner Detector.
The package InDetParticleCreation contains the top algorithm InDet::ParticleCreator which creates Inner Detector Rec::TrackParticles from
Trk::Tracks and the vertex position stored in a Trk::VxCandidate and writes them to storegate.

It uses the Trk::ParticleCreatorTool from the package TrkParticleCreator.

@section UsedPackages
@htmlinclude used_packages.html

@section Requirements
@include requirements

*/
