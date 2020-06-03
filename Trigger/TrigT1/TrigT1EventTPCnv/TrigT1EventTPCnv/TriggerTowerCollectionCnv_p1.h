/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TriggerTowerCollectionCNV_P1_H
#define TriggerTowerCollectionCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/TriggerTowerCollection.h"

// Local include(s):
#include "TrigT1EventTPCnv/TriggerTowerCollection_p1.h"
#include "TrigT1EventTPCnv/TriggerTowerCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for TriggerTower and TriggerTower_p1
 *
 *          Simple converter that can create TriggerTower objects from
 *          TriggerTower_p1 ones, and vice versa. It is used in TriggerTowerCnv
 *          to do the T/P conversion before/after writing/reading the TriggerTower
 *          object.
 */
class TriggerTowerCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< TriggerTowerCollection, TriggerTowerCollection_p1, TriggerTowerCnv_p1 > {

public:
  TriggerTowerCollectionCnv_p1() {};

}; // class TriggerTowerCollectionCnv_p1

#endif // TriggerTowerCollectionCNV_P1_H
