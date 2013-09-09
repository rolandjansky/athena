/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_TECNV_P1_H
#define TRIGMON_TECNV_P1_H

#include "TrigMonitoringEvent/TrigMonTE.h"
#include "TrigMonitoringEventTPCnv/TrigMonTE_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigMonTECnv_p1 : public T_AthenaPoolTPCnvBase<TrigMonTE, TrigMonTE_p1>
{
 public:
  
  TrigMonTECnv_p1() {}
  virtual ~TrigMonTECnv_p1() {}
  
  void persToTrans(const TrigMonTE_p1* persObj,
		   TrigMonTE* transObj,
		   MsgStream &log);
  
  void transToPers(const TrigMonTE* transObj,
		   TrigMonTE_p1* persObj,
		   MsgStream &log);
};

#endif

