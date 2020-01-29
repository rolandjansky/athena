/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROBDATACNV_P1_H
#define TRIGMON_ROBDATACNV_P1_H

#include "TrigMonitoringEvent/TrigMonROBData.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBData_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigMonROBDataCnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigMonROBData, TrigMonROBData_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigMonROBDataCnv_p1() {}
  virtual ~TrigMonROBDataCnv_p1() {}

  virtual
  void persToTrans(const TrigMonROBData_p1* persObj,
		   TrigMonROBData* transObj,
		   MsgStream &log) const override;
  
  virtual
  void transToPers(const TrigMonROBData* transObj,
		   TrigMonROBData_p1* persObj,
		   MsgStream &log) const override;
};

#endif

