/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLocRecoEventCnv_p1_h
#define AFP_SIDLocRecoEventCnv_p1_h

#include "AFP_EventTPCnv/AFP_SIDLocRecoEvent_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class AFP_SIDLocRecoEventCnv_p1
    : public T_AthenaPoolTPCnvBase< AFP_SIDLocRecoEvent, AFP_SIDLocRecoEvent_p1> 
{
public:
  AFP_SIDLocRecoEventCnv_p1() {}
  
  virtual void persToTrans( const AFP_SIDLocRecoEvent_p1* persObj, AFP_SIDLocRecoEvent* transObj, MsgStream &log);
  virtual void transToPers( const AFP_SIDLocRecoEvent* transObj, AFP_SIDLocRecoEvent_p1* persObj, MsgStream &log);
 
};
 
 
#endif
