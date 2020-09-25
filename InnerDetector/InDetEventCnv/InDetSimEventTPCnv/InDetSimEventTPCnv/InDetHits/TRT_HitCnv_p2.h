/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_HITCNV_P2_H
#define TRT_HITCNV_P2_H

/*
Transient/Persistent converter for TRT_Hit class
Author: Davide Costanzo
*/

#include "InDetSimEvent/TRTUncompressedHit.h"
#include "TRT_Hit_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TRT_HitCnv_p2  : public T_AthenaPoolTPCnvBase<TRTUncompressedHit, TRT_Hit_p2>
{
public:

  TRT_HitCnv_p2() {}

  virtual void          persToTrans(const TRT_Hit_p2* persObj, TRTUncompressedHit*
transObj, MsgStream &log);
  virtual void          transToPers(const TRTUncompressedHit* transObj, TRT_Hit_p2*
persObj, MsgStream &log);
};


#endif
