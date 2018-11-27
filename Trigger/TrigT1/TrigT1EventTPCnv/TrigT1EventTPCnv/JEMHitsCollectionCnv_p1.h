/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef JEMHitsCollectionCNV_P1_H
#define JEMHitsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/JEMHitsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/JEMHitsCollection_p1.h"
#include "TrigT1EventTPCnv/JEMHitsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for JEMHits and JEMHits_p1
 *
 *          Simple converter that can create JEMHits objects from
 *          JEMHits_p1 ones, and vice versa. It is used in JEMHitsCnv
 *          to do the T/P conversion before/after writing/reading the JEMHits
 *          object.
 */
class JEMHitsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< JEMHitsCollection, JEMHitsCollection_p1, JEMHitsCnv_p1 > {

public:
  JEMHitsCollectionCnv_p1() {};

}; // class JEMHitsCollectionCnv_p1

#endif // JEMHitsCollectionCNV_P1_H
