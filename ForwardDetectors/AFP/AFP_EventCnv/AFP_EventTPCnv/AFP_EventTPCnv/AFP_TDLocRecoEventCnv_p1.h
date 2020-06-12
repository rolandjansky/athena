/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLocRecoEventCnv_p1_h
#define AFP_TDLocRecoEventCnv_p1_h

#include "AFP_EventTPCnv/AFP_TDLocRecoEvent_p1.h"
#include "AFP_LocRecoEv/AFP_TDLocRecoEvent.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class AFP_TDLocRecoEventCnv_p1
    : public T_AthenaPoolTPCnvConstBase< AFP_TDLocRecoEvent, AFP_TDLocRecoEvent_p1> 
{
public:

  using base_class::transToPers;
  using base_class::persToTrans;



  AFP_TDLocRecoEventCnv_p1() {}
  
  virtual void persToTrans( const AFP_TDLocRecoEvent_p1* persObj, AFP_TDLocRecoEvent* transObj, MsgStream &log) const override;
  virtual void transToPers( const AFP_TDLocRecoEvent* transObj, AFP_TDLocRecoEvent_p1* persObj, MsgStream &log)const override;
 
};
 
 
#endif
