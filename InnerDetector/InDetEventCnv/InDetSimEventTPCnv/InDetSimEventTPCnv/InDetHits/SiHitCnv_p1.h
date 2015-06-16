/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHITCNV_P1_H
#define SIHITCNV_P1_H

/*
Transient/Persistent converter for SiHit class
Author: Davide Costanzo
*/

#include "InDetSimEvent/SiHit.h"
#include "SiHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class SiHitCnv_p1  : public T_AthenaPoolTPCnvBase<SiHit, SiHit_p1>
{
public:

  SiHitCnv_p1() {}

  virtual void          persToTrans(const SiHit_p1* persObj, SiHit* 
transObj, MsgStream &log);
  virtual void          transToPers(const SiHit* transObj, SiHit_p1* 
persObj, MsgStream &log);
};


#endif
