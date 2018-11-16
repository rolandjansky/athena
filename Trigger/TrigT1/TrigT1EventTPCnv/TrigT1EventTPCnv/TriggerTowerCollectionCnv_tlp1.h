/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TriggerTowerCollectionCNV_TLP1_H
#define TriggerTowerCollectionCNV_TLP1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigT1EventTPCnv/TriggerTowerCollection_tlp1.h"
#include "TrigT1EventTPCnv/TriggerTowerCollectionCnv_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for TriggerTower and TriggerTower_p1
 *
 *          Simple converter that can create TriggerTower objects from
 *          TriggerTower_p1 ones, and vice versa. It is used in TriggerTowerCnv
 *          to do the T/P conversion before/after writing/reading the TriggerTower
 *          object.
 */
class TriggerTowerCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TriggerTowerCollectionCnv_p1, TriggerTowerCollection_tlp1 > {

public:
  TriggerTowerCollectionCnv_tlp1();
  virtual ~TriggerTowerCollectionCnv_tlp1() {};
  //default methods  
  virtual void  setPStorage( TriggerTowerCollection_tlp1 *storage );

protected:
   TriggerTowerCollectionCnv_p1  m_TriggerTowerCollectionCnv;
   TriggerTowerCnv_p1            m_TriggerTowerCnv;

}; // class TriggerTowerCollectionCnv_tlp1

#endif // TriggerTowerCollectionCNV_TLP1_H
