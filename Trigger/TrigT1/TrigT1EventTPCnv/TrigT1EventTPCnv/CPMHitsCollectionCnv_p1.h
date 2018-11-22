/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMHitsCollectionCNV_P1_H
#define CPMHitsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CPMHitsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMHitsCollection_p1.h"
#include "TrigT1EventTPCnv/CPMHitsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMHits and CPMHits_p1
 *
 *          Simple converter that can create CPMHits objects from
 *          CPMHits_p1 ones, and vice versa. It is used in CPMHitsCnv
 *          to do the T/P conversion before/after writing/reading the CPMHits
 *          object.
 */
class CPMHitsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CPMHitsCollection, CPMHitsCollection_p1, CPMHitsCnv_p1 > {

public:
  CPMHitsCollectionCnv_p1() {};

}; // class CPMHitsCollectionCnv_p1

#endif // CPMHitsCollectionCNV_P1_H
