/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
  
  
#ifndef CMXRoICnv_p1_H
#define CMXRoICnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXRoI.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXRoI_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CMXRoI class */

class CMXRoICnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CMXRoI, CMXRoI_p1 > {

public:
  CMXRoICnv_p1() : m_CMXRoICnv(0) {}

  virtual void persToTrans( const CMXRoI_p1* persObj, LVL1::CMXRoI* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CMXRoI* transObj, CMXRoI_p1* persObj, MsgStream &log );

protected:
  CMXRoICnv_p1 *m_CMXRoICnv ;

}; // class CMXRoICnv_p1

#endif
