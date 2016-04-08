/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrEventCnv_p1_h
#define ALFA_LocRecCorrEventCnv_p1_h

#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvent_p1.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_LocRecCorrEventCnv_p1
    : public T_AthenaPoolTPCnvBase< ALFA_LocRecCorrEvent, ALFA_LocRecCorrEvent_p1> 
{
public:
  ALFA_LocRecCorrEventCnv_p1() {}
  
  virtual void persToTrans( const ALFA_LocRecCorrEvent_p1* persObj, ALFA_LocRecCorrEvent* transObj, MsgStream &log);
  virtual void transToPers( const ALFA_LocRecCorrEvent* transObj, ALFA_LocRecCorrEvent_p1* persObj, MsgStream &log);
 
};
 
 
#endif
