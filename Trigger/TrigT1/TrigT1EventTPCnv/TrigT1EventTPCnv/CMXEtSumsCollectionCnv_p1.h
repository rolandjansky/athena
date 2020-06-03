/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXEtSumsCollectionCNV_P1_H
#define CMXEtSumsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXEtSumsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/CMXEtSumsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXEtSums and CMXEtSums_p1
 *
 *          Simple converter that can create CMXEtSums objects from
 *          CMXEtSums_p1 ones, and vice versa. It is used in CMXEtSumsCnv
 *          to do the T/P conversion before/after writing/reading the CMXEtSums
 *          object.
 */
class CMXEtSumsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CMXEtSumsCollection, CMXEtSumsCollection_p1, CMXEtSumsCnv_p1 > {

public:
  CMXEtSumsCollectionCnv_p1() {};

}; // class CMXEtSumsCollectionCnv_p1

#endif // CMXEtSumsCollectionCNV_P1_H
