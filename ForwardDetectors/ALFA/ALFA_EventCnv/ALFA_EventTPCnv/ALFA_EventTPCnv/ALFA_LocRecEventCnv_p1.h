/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecEventCnv_p1_h
#define ALFA_LocRecEventCnv_p1_h

#include "ALFA_EventTPCnv/ALFA_LocRecEvent_p1.h"
#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_LocRecEventCnv_p1
    : public T_AthenaPoolTPCnvConstBase< ALFA_LocRecEvent, ALFA_LocRecEvent_p1>
{
public:
  using base_class::persToTrans;
  using base_class::transToPers;

  ALFA_LocRecEventCnv_p1() {}
  
  virtual void persToTrans( const ALFA_LocRecEvent_p1* persObj, ALFA_LocRecEvent* transObj, MsgStream &log) const override;
  virtual void transToPers( const ALFA_LocRecEvent* transObj, ALFA_LocRecEvent_p1* persObj, MsgStream &log) const override;
 
};
 
 
#endif
