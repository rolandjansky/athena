/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROBDATACNV_P2_H
#define TRIGMON_ROBDATACNV_P2_H

#include "TrigMonitoringEvent/TrigMonROBData.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBData_p2.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigMonROBDataCnv_p2 : public T_AthenaPoolTPCnvBase<TrigMonROBData, TrigMonROBData_p2>
{
 public:
  
  TrigMonROBDataCnv_p2() {}
  virtual ~TrigMonROBDataCnv_p2() {}
  
  void persToTrans(const TrigMonROBData_p2* persObj,
		   TrigMonROBData* transObj,
		   MsgStream &log);
  
  void transToPers(const TrigMonROBData* transObj,
		   TrigMonROBData_p2* persObj,
		   MsgStream &log);
};

#endif

