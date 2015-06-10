/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHITCNV_P1_H
#define LARHITCNV_P1_H

/*
Transient/Persistent converter for LArHit class
Author: Davide Costanzo
*/

#include "LArSimEvent/LArHit.h"
#include "LArSimEventTPCnv/LArHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class LArHitCnv_p1  : public T_AthenaPoolTPCnvBase<LArHit, LArHit_p1>
{
public:

  LArHitCnv_p1() {}

  virtual void          persToTrans(const LArHit_p1* persObj, LArHit* transObj, MsgStream &log);
  virtual void          transToPers(const LArHit* transObj, LArHit_p1* persObj, MsgStream &log);
};


#endif
