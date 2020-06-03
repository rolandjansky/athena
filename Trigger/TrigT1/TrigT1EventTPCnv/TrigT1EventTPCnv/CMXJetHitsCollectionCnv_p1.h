/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXJetHitsCollectionCNV_P1_H
#define CMXJetHitsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXJetHitsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXJetHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXJetHitsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXJetHits and CMXJetHits_p1
 *
 *          Simple converter that can create CMXJetHits objects from
 *          CMXJetHits_p1 ones, and vice versa. It is used in CMXJetHitsCnv
 *          to do the T/P conversion before/after writing/reading the CMXJetHits
 *          object.
 */
class CMXJetHitsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CMXJetHitsCollection, CMXJetHitsCollection_p1, CMXJetHitsCnv_p1 > {

public:
  CMXJetHitsCollectionCnv_p1() {};

}; // class CMXJetHitsCollectionCnv_p1

#endif // CMXJetHitsCollectionCNV_P1_H
