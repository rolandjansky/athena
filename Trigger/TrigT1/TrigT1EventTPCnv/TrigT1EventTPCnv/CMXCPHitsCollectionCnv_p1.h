/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXCPHitsCollectionCNV_P1_H
#define CMXCPHitsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXCPHitsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXCPHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXCPHitsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXCPHits and CMXCPHits_p1
 *
 *          Simple converter that can create CMXCPHits objects from
 *          CMXCPHits_p1 ones, and vice versa. It is used in CMXCPHitsCnv
 *          to do the T/P conversion before/after writing/reading the CMXCPHits
 *          object.
 */
class CMXCPHitsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CMXCPHitsCollection, CMXCPHitsCollection_p1, CMXCPHitsCnv_p1 > {

public:
  CMXCPHitsCollectionCnv_p1() {};

}; // class CMXCPHitsCollectionCnv_p1

#endif // CMXCPHitsCollectionCNV_P1_H
