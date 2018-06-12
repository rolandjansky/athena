/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMTowerCollectionCNV_P1_H
#define CPMTowerCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CPMTowerCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMTowerCollection_p1.h"
#include "TrigT1EventTPCnv/CPMTowerCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMTower and CPMTower_p1
 *
 *          Simple converter that can create CPMTower objects from
 *          CPMTower_p1 ones, and vice versa. It is used in CPMTowerCnv
 *          to do the T/P conversion before/after writing/reading the CPMTower
 *          object.
 */
class CPMTowerCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CPMTowerCollection, CPMTowerCollection_p1, CPMTowerCnv_p1 > {

public:
  CPMTowerCollectionCnv_p1() {};

}; // class CPMTowerCollectionCnv_p1

#endif // CPMTowerCollectionCNV_P1_H
