/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMCPHitsCollectionCNV_P1_H
#define CMMCPHitsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMMCPHitsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMMCPHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMCPHitsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMMCPHits and CMMCPHits_p1
 *
 *          Simple converter that can create CMMCPHits objects from
 *          CMMCPHits_p1 ones, and vice versa. It is used in CMMCPHitsCnv
 *          to do the T/P conversion before/after writing/reading the CMMCPHits
 *          object.
 */
class CMMCPHitsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CMMCPHitsCollection, CMMCPHitsCollection_p1, CMMCPHitsCnv_p1 > {

public:
  CMMCPHitsCollectionCnv_p1() {};

}; // class CMMCPHitsCollectionCnv_p1

#endif // CMMCPHitsCollectionCNV_P1_H
