/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMRoICollectionCNV_P1_H
#define CPMRoICollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CPMRoICollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMRoICollection_p1.h"
#include "TrigT1EventTPCnv/CPMRoICnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMRoI and CPMRoI_p1
 *
 *          Simple converter that can create CPMRoI objects from
 *          CPMRoI_p1 ones, and vice versa. It is used in CPMRoICnv
 *          to do the T/P conversion before/after writing/reading the CPMRoI
 *          object.
 */
class CPMRoICollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CPMRoICollection, CPMRoICollection_p1, CPMRoICnv_p1 > {

public:
  CPMRoICollectionCnv_p1() {};

}; // class CPMRoICollectionCnv_p1

#endif // CPMRoICollectionCNV_P1_H
