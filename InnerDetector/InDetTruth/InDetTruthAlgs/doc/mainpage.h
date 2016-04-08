/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage InDetTruthAlgs Package
@author Maria.Jose.Costa@cern.ch, Andreas.Wildauer@cern.ch

@section InDetTruthAlgsIntro Introduction

This package contains the algorithms to create the truth objects and the
maps objects that connect the truth and the reconstructed objects.

For each of the reconstructed objects (PrepRawData, SpacePoint, Track and
TrackParticle) there is a truth object associated with it. The truth object is 
meant to point to the source that contributed the most to the creation of 
that object, where source here stands for either noise or a generated 
particle. It contains then a HepMcParticleLink object (dominant source) 
and a probability to indicate what was the contribution of this source to 
the creation of the reconstructed object. 

Note that a decision is taken in the algorithms filling the truth since 
the information of all the other sources that also contribute is lost.

@section InDetTruthAlgsOverview Algorithm Overview
  The InDetTruthAlgs package contains the following algorithms:

   - PRD_MultiTruthMaker: Creates multimaps of PrepRawData (PRD_MultiTruthCollection) to their MC particles.

   - InDetDetailedTrackTruthMaker: For each reconstructed Track it
     creates corresponding DetailedTrackTruth objects and fills
     DetailedTrackTruthCollection. It is based on the truth of all the
     PrepRawData objects contained in the Track.

   - TrackParticleTruthMaker: For each TrackParticle it creates the
   TrackParticleTruth and fills both objects in a map. It is based on
   the truth of the Track contained in the TrackParticle.

@section InDetTruthAlgsReq Requirements

@include requirements
*/
