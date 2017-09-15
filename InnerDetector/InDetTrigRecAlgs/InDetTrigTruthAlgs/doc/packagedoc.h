/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetTrigTruthAlgs_page InDetTrigTruthAlgs Package equivalent of InDetTruthAlgs in the offline
@author Maria.Jose.Costa@cern.ch, Andreas.Wildauer@cern.ch
@author Iwona.Grabowska@cern.ch (adapted to the Event Filter trigger)

@section InDetTrigTruthAlgs_InDetTrigTruthAlgsIntro Introduction

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

@section InDetTrigTruthAlgs_InDetTrigTruthAlgsOverview Algorithm Overview
  The InDetTrigTruthAlgs package contains the following algorithms:

   - InDetPrepRawDataTruthMaker called directly from the offline by
   means of TrigEFWrappers: For each PrepRawData objects it creates
   the PrepRawDataTruth objects and fills both of them in a map. It is
   based on the truth of the RawDataObjects which are contained in the
   given PrepRawData.

   - SpacePointTruthMaker called directly from the offline by means of
   TrigEFWrappers: For each SpacePoint it creates the SpacePointTruth
   object fills both of them in a map. It is based on the truth of the
   PrepRawData objects contained in a SpacePoint.

   - InDetTrigTrackTruthMaker: For each reconstructed Track it creates the 
   TrackTruth object and fills both of them in a map. It is based on the
   truth of all the PrepRawData objects contained in the Track.

   - TrigTrackParticleTruthMaker: For each TrackParticle it creates the
   TrackParticleTruth and fills both objects in a map. It is based on
   the truth of the Track contained in the TrackParticle.


*/
