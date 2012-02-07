/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLETRUTH_TRKPARTICLETRUTHCOLLECTIONCONT_H
#define PARTICLETRUTH_TRKPARTICLETRUTHCOLLECTIONCONT_H

/*
  @brief TrackParticleTruthCollectionContainer is used in the trigger navigation
  @author Jiri.Masik@cern.ch
*/

#include "CLIDSvc/CLASS_DEF.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"

typedef DataVector<TrackParticleTruthCollection> TrackParticleTruthCollectionContainer;
//class TrackParticleTruthCollectionContainer : public DataVector<TrackParticleTruthCollection>{};
CLASS_DEF( TrackParticleTruthCollectionContainer , 1224650254 , 1 )

#endif







