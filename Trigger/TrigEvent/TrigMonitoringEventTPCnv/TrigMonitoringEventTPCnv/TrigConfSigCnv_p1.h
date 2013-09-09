/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_SIGCNV_P1_H
#define TRIGCONF_SIGCNV_P1_H

#include "TrigMonitoringEvent/TrigConfSig.h"
#include "TrigMonitoringEventTPCnv/TrigConfSig_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigConfSigCnv_p1 : public T_AthenaPoolTPCnvBase<TrigConfSig, TrigConfSig_p1>
{
 public:
  
  TrigConfSigCnv_p1() {}
  virtual ~TrigConfSigCnv_p1() {}
  
  void persToTrans(const TrigConfSig_p1* persObj,
		   TrigConfSig* transObj,
		   MsgStream &log);
  
  void transToPers(const TrigConfSig* transObj,
		   TrigConfSig_p1* persObj,
		   MsgStream &log);
};

#endif

