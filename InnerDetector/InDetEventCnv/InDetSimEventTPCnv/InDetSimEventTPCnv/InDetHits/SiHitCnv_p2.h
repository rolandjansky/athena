/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHITCNV_P2_H
#define SIHITCNV_P2_H

/*
Transient/Persistent converter for SiHit class
Author: Davide Costanzo
*/

#include "InDetSimEvent/SiHit.h"
#include "SiHit_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class SiHitCnv_p2  : public T_AthenaPoolTPCnvBase<SiHit, SiHit_p2>
{
public:

  SiHitCnv_p2() {}

  virtual void          persToTrans(const SiHit_p2* persObj, SiHit*
transObj, MsgStream &log);
  virtual void          transToPers(const SiHit* transObj, SiHit_p2*
persObj, MsgStream &log);
};


#endif
