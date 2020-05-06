/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef TRACKTRUTHCOLLECTIONCNV_H
#define TRACKTRUTHCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthTPCnv/TrackTruthCollection_p3.h"
#include "TrkTruthTPCnv/TrackTruthCollection_p2.h"
#include "TrkTruthTPCnv/TrackTruthCollection_p1.h"
#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p3.h"
#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p2.h"
#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p1.h"
#include "TrkTruthTPCnv/TrackTruthCollectionCnv_p0.h"

namespace Trk { class TrackTruthCollection_p3; }

typedef Trk::TrackTruthCollection_p3 TrackTruthCollectionPERS;

typedef T_AthenaPoolCustomCnv<TrackTruthCollection, TrackTruthCollectionPERS> TrackTruthCollectionCnvBase;

class TrackTruthCollectionCnv : public TrackTruthCollectionCnvBase
{
  friend class CnvFactory<TrackTruthCollectionCnv>;
protected:
   TrackTruthCollectionCnv(ISvcLocator* svcloc);
  virtual TrackTruthCollection* createTransient();
  virtual TrackTruthCollectionPERS* createPersistent(TrackTruthCollection*);
private:
  TrackTruthCollectionCnv_p0 m_converter_p0;
  TrackTruthCollectionCnv_p1 m_converter_p1;
  TrackTruthCollectionCnv_p2 m_converter_p2;
  TrackTruthCollectionCnv_p3 m_converter_p3;

  static pool::Guid p0_guid;
  static pool::Guid p1_guid;
  static pool::Guid p2_guid;
  static pool::Guid p3_guid;
};

#endif
