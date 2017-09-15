/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrigParticleCreation_page InDetTrigParticleCreation

@section InDetTrigParticleCreation_introduction Introduction:

InDetTrigParticleCreation is the last algorithm of the Event Filter
InnerDetector software chain. It combines information from previous
algorithms and creates TrackParticles.

@section InDetTrigParticleCreation_howitworks How it works:

The only algorithm of the package is TrigParticleCreator. For each RoI
it reads all tracks from StoreGate, all tracks used in the primary
vertex fit and creates TrackParticles.  The TrackParticle is always
constructed with the primary vertex. If its track has been used for
the primary vertex fit the TrackParticleOrigin enum will be set to
Rec::PriVtx, if not it will be set to Rec::NoVtx.

@section InDetTrigParticleCreation_packagecontent Package Contents:
InDetTrigParticleCreation contains the following files/classes:
- TrigParticleCreator.cxx ... the algoritm which creates particles



\namespace PCreate
*/
