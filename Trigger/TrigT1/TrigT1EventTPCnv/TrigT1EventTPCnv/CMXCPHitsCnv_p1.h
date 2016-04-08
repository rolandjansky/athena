
 //***************************************************************************
 //                           CMXCPHitsCnv_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     copyright            : (C) 2008 by Alan Watson
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
#ifndef CMXCPHitsCnv_p1_H
#define CMXCPHitsCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXCPHits.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXCPHits_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CMXCPHits class */

class CMXCPHitsCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CMXCPHits, CMXCPHits_p1 > {

public:
  CMXCPHitsCnv_p1() : m_CMXCPHitsCnv(0) {}

  virtual void persToTrans( const CMXCPHits_p1* persObj, LVL1::CMXCPHits* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CMXCPHits* transObj, CMXCPHits_p1* persObj, MsgStream &log );

protected:
  CMXCPHitsCnv_p1 *m_CMXCPHitsCnv ;

}; // class TriggerTowerCnv_p1

#endif
