/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMTobRoICollectionCNV_P1_H
#define CPMTobRoICollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CPMTobRoICollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMTobRoICollection_p1.h"
#include "TrigT1EventTPCnv/CPMTobRoICnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMTobRoI and CPMTobRoI_p1
 *
 *          Simple converter that can create CPMTobRoI objects from
 *          CPMTobRoI_p1 ones, and vice versa. It is used in CPMTobRoICnv
 *          to do the T/P conversion before/after writing/reading the CPMTobRoI
 *          object.
 */
class CPMTobRoICollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CPMTobRoICollection, CPMTobRoICollection_p1, CPMTobRoICnv_p1 > {

public:
  CPMTobRoICollectionCnv_p1() {};

}; // class CPMTobRoICollectionCnv_p1

#endif // CPMTobRoICollectionCNV_P1_H
