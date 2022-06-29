/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrODEventCnv_p1_h
#define ALFA_LocRecCorrODEventCnv_p1_h

#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvent_p1.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvent.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
class MsgStream;

class ALFA_LocRecCorrODEventCnv_p1
    : public T_AthenaPoolTPCnvConstBase< ALFA_LocRecCorrODEvent, ALFA_LocRecCorrODEvent_p1>
{
public:
  using base_class::persToTrans;
  using base_class::transToPers;

  ALFA_LocRecCorrODEventCnv_p1() {}
  
  virtual void persToTrans( const ALFA_LocRecCorrODEvent_p1* persObj, ALFA_LocRecCorrODEvent* transObj, MsgStream &log) const override;
  virtual void transToPers( const ALFA_LocRecCorrODEvent* transObj, ALFA_LocRecCorrODEvent_p1* persObj, MsgStream &log) const override;
 
};
 
 
#endif
