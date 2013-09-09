/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ALGCNV_P1_H
#define TRIGMON_ALGCNV_P1_H

#include "TrigMonitoringEvent/TrigMonAlg.h"
#include "TrigMonitoringEventTPCnv/TrigMonAlg_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigMonAlgCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMonAlg, TrigMonAlg_p1>
{
 public:
  
  TrigMonAlgCnv_p1() {}
  virtual ~TrigMonAlgCnv_p1() {}
  
  void persToTrans(const TrigMonAlg_p1* persObj,
		   TrigMonAlg* transObj,
		   MsgStream &log);
  
  void transToPers(const TrigMonAlg* transObj,
		   TrigMonAlg_p1* persObj,
		   MsgStream &log);
};

#endif

