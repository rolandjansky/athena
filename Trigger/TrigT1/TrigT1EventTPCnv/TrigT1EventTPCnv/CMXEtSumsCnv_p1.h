/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
  
  
#ifndef CMXEtSumsCnv_p1_H
#define CMXEtSumsCnv_p1_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Trigger include(s):
#include "TrigT1CaloEvent/CMXEtSums.h"

// Local include(s):
#include "TrigT1EventTPCnv/CMXEtSums_p1.h"

//using namespace LVL1;

/** T/P convertor for TrigT1Calo CMXEtSums class */

class CMXEtSumsCnv_p1 : public T_AthenaPoolTPCnvBase< LVL1::CMXEtSums, CMXEtSums_p1 > {

public:
  CMXEtSumsCnv_p1() : m_CMXEtSumsCnv(0) {}

  virtual void persToTrans( const CMXEtSums_p1* persObj, LVL1::CMXEtSums* transObj, MsgStream &log );
  virtual void transToPers( const LVL1::CMXEtSums* transObj, CMXEtSums_p1* persObj, MsgStream &log );

protected:
  CMXEtSumsCnv_p1 *m_CMXEtSumsCnv ;

}; // class CMXEtSumsCnv_p1

#endif
