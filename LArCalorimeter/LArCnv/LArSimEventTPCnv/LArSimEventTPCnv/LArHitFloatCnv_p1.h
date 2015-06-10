/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHITFLOATCNV_P1_H
#define LARHITFLOATCNV_P1_H

/*
Transient/Persistent converter for LArHit class
Author: Davide Costanzo
*/

#include "LArSimEvent/LArHitFloat.h"
#include "LArSimEventTPCnv/LArHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class LArHitFloatCnv_p1  : public T_AthenaPoolTPCnvBase<LArHitFloat, LArHit_p1>
{
public:

  LArHitFloatCnv_p1() {}

  virtual void          persToTrans(const LArHit_p1* persObj, LArHitFloat* transObj, MsgStream &log);
  virtual void          transToPers(const LArHitFloat* transObj, LArHit_p1* persObj, MsgStream &log);
};


#endif
