#ifndef TriggerTowerCNV_P1_H
#define TriggerTowerCNV_P1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Local include(s):
#include "TrigT1EventTPCnv/TriggerTower_p1.h"

using namespace LVL1;

/**
 *   @short T/P converter for TriggerTower and TriggerTower_p1
 *
 *          Simple converter that can create TriggerTower objects from
 *          TriggerTower_p1 ones, and vice versa. It is used in TriggerTowerCnv
 *          to do the T/P conversion before/after writing/reading the TriggerTower
 *          object.
 */
class TriggerTowerCnv_p1 : public T_AthenaPoolTPCnvBase< TriggerTower, TriggerTower_p1 > {

public:
  TriggerTowerCnv_p1() : m_TriggerTowerCnv(0) {}

  virtual void persToTrans( const TriggerTower_p1* persObj, TriggerTower* transObj, MsgStream &log );
  virtual void transToPers( const TriggerTower* transObj, TriggerTower_p1* persObj, MsgStream &log );

protected:
  TriggerTowerCnv_p1 *m_TriggerTowerCnv ;

}; // class TriggerTowerCnv_p1

#endif // TriggerTowerCNV_P1_H
