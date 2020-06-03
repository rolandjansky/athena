/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CPMTowerCNV_P1_H
#define CPMTowerCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CPMTower.h"

// Local include(s):
#include "TrigT1EventTPCnv/CPMTower_p1.h"

//using namespace LVL1;

/**
 *   @short T/P converter for CPMTower and CPMTower_p1
 *
 *          Simple converter that can create CPMTower objects from
 *          CPMTower_p1 ones, and vice versa. It is used in CPMTowerCnv
 *          to do the T/P conversion before/after writing/reading the CPMTower
 *          object.
 */
class CPMTowerCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CPMTower, CPMTower_p1 > {

public:
  CPMTowerCnv_p1() : m_CPMTowerCnv(0) {}

  virtual void persToTrans( const CPMTower_p1* persObj, LVL1::CPMTower* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CPMTower* transObj, CPMTower_p1* persObj, MsgStream &log );

protected:
  CPMTowerCnv_p1 *m_CPMTowerCnv ;

}; // class CPMTowerCnv_p1

#endif // CPMTowerCNV_P1_H
