/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKPARTICLETRUTHCOLLECTIONCNV_H
#define TRACKPARTICLETRUTHCOLLECTIONCNV_H
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "ParticleTruth/TrackParticleTruthCollection.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollection_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p2.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p1.h"
#include "TrackParticleTruthTPCnv/TrackParticleTruthCollectionCnv_p0.h"

namespace Trk { class TrackParticleTruthCollection_p2; }

typedef Rec::TrackParticleTruthCollection_p2 TrackParticleTruthCollectionPERS;

typedef T_AthenaPoolCustomCnv<TrackParticleTruthCollection, TrackParticleTruthCollectionPERS> TrackParticleTruthCollectionCnvBase;

class TrackParticleTruthCollectionCnv : public TrackParticleTruthCollectionCnvBase
{
  friend class CnvFactory<TrackParticleTruthCollectionCnv>;
protected:
public:
   TrackParticleTruthCollectionCnv(ISvcLocator* svcloc);
protected:
  virtual TrackParticleTruthCollection* createTransient();
  virtual TrackParticleTruthCollectionPERS* createPersistent(TrackParticleTruthCollection*);
private:
  TrackParticleTruthCollectionCnv_p0 m_converter_p0;
  TrackParticleTruthCollectionCnv_p1 m_converter_p1;
  TrackParticleTruthCollectionCnv_p2 m_converter_p2;

  static pool::Guid s_p0_guid;
  static pool::Guid s_p1_guid;
  static pool::Guid s_p2_guid;
};

#endif
