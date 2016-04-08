/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecODEventCnv_p1_h
#define ALFA_LocRecODEventCnv_p1_h

#include "ALFA_EventTPCnv/ALFA_LocRecODEvent_p1.h"
#include "ALFA_LocRecEv/ALFA_LocRecODEvent.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_LocRecODEventCnv_p1
    : public T_AthenaPoolTPCnvBase< ALFA_LocRecODEvent, ALFA_LocRecODEvent_p1> 
{
public:
  ALFA_LocRecODEventCnv_p1() {}
  
  virtual void persToTrans( const ALFA_LocRecODEvent_p1* persObj, ALFA_LocRecODEvent* transObj, MsgStream &log);
  virtual void transToPers( const ALFA_LocRecODEvent* transObj, ALFA_LocRecODEvent_p1* persObj, MsgStream &log);
 
};
 
 
#endif
