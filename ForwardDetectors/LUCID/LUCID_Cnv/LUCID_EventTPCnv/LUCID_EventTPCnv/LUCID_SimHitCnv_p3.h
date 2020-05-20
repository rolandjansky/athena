/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_SIMHITCNV_P3_H
#define LUCID_SIMHITCNV_P3_H

#include "LUCID_EventTPCnv/LUCID_SimHit_p3.h"
#include "LUCID_SimEvent/LUCID_SimHit.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_SimHitCnv_p3: public T_AthenaPoolTPCnvBase<LUCID_SimHit, LUCID_SimHit_p3> {

 public:
  
  LUCID_SimHitCnv_p3() {}
  
  virtual void persToTrans(const LUCID_SimHit_p3* persObj, LUCID_SimHit*    transObj, MsgStream& log);
  virtual void transToPers(const LUCID_SimHit*   transObj, LUCID_SimHit_p3* persObj , MsgStream& log);
};


#endif
