/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMJetHitsCollectionCNV_P1_H
#define CMMJetHitsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMMJetHitsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMMJetHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMJetHitsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMMJetHits and CMMJetHits_p1
 *
 *          Simple converter that can create CMMJetHits objects from
 *          CMMJetHits_p1 ones, and vice versa. It is used in CMMJetHitsCnv
 *          to do the T/P conversion before/after writing/reading the CMMJetHits
 *          object.
 */
class CMMJetHitsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CMMJetHitsCollection, CMMJetHitsCollection_p1, CMMJetHitsCnv_p1 > {

public:
  CMMJetHitsCollectionCnv_p1() {};

}; // class CMMJetHitsCollectionCnv_p1

#endif // CMMJetHitsCollectionCNV_P1_H
