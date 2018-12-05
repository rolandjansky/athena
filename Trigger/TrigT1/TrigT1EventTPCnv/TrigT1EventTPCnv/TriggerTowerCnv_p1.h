/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TriggerTowerCNV_P1_H
#define TriggerTowerCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/TriggerTower.h"

// Local include(s):
#include "TrigT1EventTPCnv/TriggerTower_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for TriggerTower and TriggerTower_p1
 *
 *          Simple converter that can create TriggerTower objects from
 *          TriggerTower_p1 ones, and vice versa. It is used in TriggerTowerCnv
 *          to do the T/P conversion before/after writing/reading the TriggerTower
 *          object.
 */
class TriggerTowerCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::TriggerTower, TriggerTower_p1 > {

public:
  TriggerTowerCnv_p1() : m_TriggerTowerCnv(0) {}

  virtual void persToTrans( const TriggerTower_p1* persObj, LVL1::TriggerTower* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::TriggerTower* transObj, TriggerTower_p1* persObj, MsgStream &log );

protected:
  TriggerTowerCnv_p1 *m_TriggerTowerCnv ;

}; // class TriggerTowerCnv_p1

#endif // TriggerTowerCNV_P1_H
