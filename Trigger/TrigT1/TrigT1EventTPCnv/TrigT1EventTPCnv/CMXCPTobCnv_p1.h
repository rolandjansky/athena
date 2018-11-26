/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CMXCPTobCnv_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CMXCPTobCnv_p1_H
#define CMXCPTobCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXCPTob.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXCPTob_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CMXCPTob class */

class CMXCPTobCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CMXCPTob, CMXCPTob_p1 > {

public:
  CMXCPTobCnv_p1() : m_CMXCPTobCnv(0) {}

  virtual void persToTrans( const CMXCPTob_p1* persObj, LVL1::CMXCPTob* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CMXCPTob* transObj, CMXCPTob_p1* persObj, MsgStream &log );

protected:
  CMXCPTobCnv_p1 *m_CMXCPTobCnv ;

}; // class TriggerTowerCnv_p1

#endif
