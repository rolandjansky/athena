/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef DETAILEDTRACKTRUTHCOLLECTIONCNV_H
#define DETAILEDTRACKTRUTHCOLLECTIONCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrkTruthData/DetailedTrackTruthCollection.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollectionCnv_p1.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollectionCnv_p2.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollectionCnv_p3.h"
#include "TrkTruthTPCnv/DetailedTrackTruthCollectionCnv_p4.h"

namespace Trk { class DetailedTrackTruthCollection_p4; }

typedef Trk::DetailedTrackTruthCollection_p4 DetailedTrackTruthCollectionPERS;

typedef T_AthenaPoolCustomCnv<DetailedTrackTruthCollection, DetailedTrackTruthCollectionPERS> DetailedTrackTruthCollectionCnvBase;

class DetailedTrackTruthCollectionCnv : public DetailedTrackTruthCollectionCnvBase
{
  friend class CnvFactory<DetailedTrackTruthCollectionCnv>;
protected:
   DetailedTrackTruthCollectionCnv(ISvcLocator* svcloc);
  virtual DetailedTrackTruthCollection* createTransient();
  virtual DetailedTrackTruthCollectionPERS* createPersistent(DetailedTrackTruthCollection*);
private:
  static pool::Guid p0_guid;
  static pool::Guid p1_guid;
  static pool::Guid p2_guid;
  static pool::Guid p3_guid;
  static pool::Guid p4_guid;
  DetailedTrackTruthCollectionCnv_p1 m_converter_p1;
  DetailedTrackTruthCollectionCnv_p2 m_converter_p2;
  DetailedTrackTruthCollectionCnv_p3 m_converter_p3;
  DetailedTrackTruthCollectionCnv_p4 m_converter_p4;
};

#endif/*DETAILEDTRACKTRUTHCOLLECTIONCNV_H*/
