  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
#ifndef CMXJetHitsCnv_p1_H
#define CMXJetHitsCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXJetHits.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXJetHits_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CMXJetHits class */

class CMXJetHitsCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CMXJetHits, CMXJetHits_p1 > {

public:
  CMXJetHitsCnv_p1() : m_CMXJetHitsCnv(0) {}

  virtual void persToTrans( const CMXJetHits_p1* persObj, LVL1::CMXJetHits* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CMXJetHits* transObj, CMXJetHits_p1* persObj, MsgStream &log );

protected:
  CMXJetHitsCnv_p1 *m_CMXJetHitsCnv ;

}; // class CMXJetHitsCnv_p1

#endif
