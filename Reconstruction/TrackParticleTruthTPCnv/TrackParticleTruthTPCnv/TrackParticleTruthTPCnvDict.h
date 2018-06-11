/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHTPCNV_H
#define TRACKPARTICLETRUTHTPCNV_H

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

// namespace {
  // --->> genreflex: WARNING: Class std::pair<ElementLink<DataVector<Rec::TrackParticle>, DataProxyStorage<DataVector<Rec::TrackParticle> >,ForwardIndexingPolicy<DataVector<Rec::TrackParticle> > >,TrackTruth> in selection file ../i686-slc4-gcc34-opt/dict/TrkTruthTPCnv_selection.xml not generated.  Fixing that:
struct GCCXML_DUMMY_INSTANTIATION_TRACKPARTICLETRUTHTPCNV {
  std::pair<ElementLink<Rec::TrackParticleContainer>,TrackParticleTruth>  dummy1;
  T_TPCnv<TrackParticleTruthCollection, Rec::TrackParticleTruthCollection_p1> dummy2;
  std::vector<Rec::TrackParticleTruthCollection_p1> aaa;
  T_TPCnv<TrackParticleTruthCollection, Rec::TrackParticleTruthCollection_p2> dummy2b;
  std::vector<Rec::TrackParticleTruthCollection_p2> aab;

  T_TPCnv<TrackParticleTruthCollectionContainer, TrackParticleTruthCollectionContainer_tlp1> dummy3;
  std::vector<TrackParticleTruthCollectionContainer_p1> abc;
  T_TPCnv<TrackParticleTruthCollectionContainer, TrackParticleTruthCollectionContainer_tlp2> dummy3b;
  std::vector<TrackParticleTruthCollectionContainer_p2> abb;
};

// }

#endif/*TRACKPARTICLETRUTHTPCNV_H*/

