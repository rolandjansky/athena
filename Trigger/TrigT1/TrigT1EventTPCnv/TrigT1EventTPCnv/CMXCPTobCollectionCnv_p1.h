/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CMXCPTobCollectionCNV_P1_H
#define CMXCPTobCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXCPTobCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXCPTobCollection_p1.h"
#include "TrigT1EventTPCnv/CMXCPTobCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CMXCPTob and CMXCPTob_p1
 *
 *          Simple converter that can create CMXCPTob objects from
 *          CMXCPTob_p1 ones, and vice versa. It is used in CMXCPTobCnv
 *          to do the T/P conversion before/after writing/reading the CMXCPTob
 *          object.
 */
class CMXCPTobCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CMXCPTobCollection, CMXCPTobCollection_p1, CMXCPTobCnv_p1 > {

public:
  CMXCPTobCollectionCnv_p1() {};

}; // class CMXCPTobCollectionCnv_p1

#endif // CMXCPTobCollectionCNV_P1_H
