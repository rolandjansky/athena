/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p0.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p2.h"
#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "ParticleTruth/TrackParticleTruthCollectionContainer.h"

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_tlp1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp1.h"

#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainer_tlp2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionContainerCnv_tlp2.h"

DECLARE_TPCNV_FACTORY(TrackParticleTruthCollectionContainerCnv_tlp1,
                      DataVector<TrackParticleTruthCollection>,
                      TrackParticleTruthCollectionContainer_tlp1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrackParticleTruthCollectionContainerCnv_tlp2,
                      DataVector<TrackParticleTruthCollection>,
                      TrackParticleTruthCollectionContainer_tlp2,
                      Athena::TPCnvVers::Current)

DECLARE_TPCNV_FACTORY(TrackParticleTruthCollectionCnv_p1,
                      TrackParticleTruthCollection,
                      Rec::TrackParticleTruthCollection_p1,
                      Athena::TPCnvVers::Old)

DECLARE_TPCNV_FACTORY(TrackParticleTruthCollectionCnv_p2,
                      TrackParticleTruthCollection,
                      Rec::TrackParticleTruthCollection_p2,
                      Athena::TPCnvVers::Current)
