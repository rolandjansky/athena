/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRecEventCnv_p1_h
#define ALFA_GloRecEventCnv_p1_h

#include "ALFA_EventTPCnv/ALFA_GloRecEvent_p1.h"
#include "ALFA_GloRecEv/ALFA_GloRecEvent.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_GloRecEventCnv_p1
    : public T_AthenaPoolTPCnvBase< ALFA_GloRecEvent, ALFA_GloRecEvent_p1> 
{
public:
  ALFA_GloRecEventCnv_p1() {}
  
  virtual void persToTrans( const ALFA_GloRecEvent_p1* persObj, ALFA_GloRecEvent* transObj, MsgStream &log);
  virtual void transToPers( const ALFA_GloRecEvent* transObj, ALFA_GloRecEvent_p1* persObj, MsgStream &log);
 
};
 
 
#endif
