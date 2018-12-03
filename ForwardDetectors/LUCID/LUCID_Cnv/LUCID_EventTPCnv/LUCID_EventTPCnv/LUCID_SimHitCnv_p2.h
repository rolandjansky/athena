/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_SIMHITCNV_P2_H
#define LUCID_SIMHITCNV_P2_H

#include "LUCID_EventTPCnv/LUCID_SimHit_p2.h"
#include "LUCID_SimEvent/LUCID_SimHit.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_SimHitCnv_p2: public T_AthenaPoolTPCnvBase<LUCID_SimHit, LUCID_SimHit_p2> {

 public:
  
  LUCID_SimHitCnv_p2() {}
  
  virtual void persToTrans(const LUCID_SimHit_p2* persObj, LUCID_SimHit*    transObj, MsgStream& log);
  virtual void transToPers(const LUCID_SimHit*   transObj, LUCID_SimHit_p2* persObj , MsgStream& log);
};


#endif
