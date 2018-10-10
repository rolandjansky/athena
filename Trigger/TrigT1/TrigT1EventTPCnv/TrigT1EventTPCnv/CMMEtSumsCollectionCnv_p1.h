/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMMEtSumsCollectionCNV_P1_H
#define CMMEtSumsCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMMEtSumsCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMMEtSumsCollection_p1.h"
#include "TrigT1EventTPCnv/CMMEtSumsCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMMEtSums and CMMEtSums_p1
 *
 *          Simple converter that can create CMMEtSums objects from
 *          CMMEtSums_p1 ones, and vice versa. It is used in CMMEtSumsCnv
 *          to do the T/P conversion before/after writing/reading the CMMEtSums
 *          object.
 */
class CMMEtSumsCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CMMEtSumsCollection, CMMEtSumsCollection_p1, CMMEtSumsCnv_p1 > {

public:
  CMMEtSumsCollectionCnv_p1() {};

}; // class CMMEtSumsCollectionCnv_p1

#endif // CMMEtSumsCollectionCNV_P1_H
