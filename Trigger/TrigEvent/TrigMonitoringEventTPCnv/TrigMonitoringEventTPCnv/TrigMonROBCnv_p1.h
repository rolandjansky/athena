/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROBCNV_P1_H
#define TRIGMON_ROBCNV_P1_H

// Framework
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Transient data
#include "TrigMonitoringEvent/TrigMonROB.h"

// Persistent data and converters
#include "TrigMonitoringEventTPCnv/TrigMonROB_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBDataCnv_p1.h"

class MsgStream;

class TrigMonROBCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMonROB, TrigMonROB_p1>
{
 public:
  
  TrigMonROBCnv_p1() {}
  virtual ~TrigMonROBCnv_p1() {}
  
  void persToTrans(const TrigMonROB_p1* persObj,
		   TrigMonROB* transObj,
		   MsgStream &log);
  
  void transToPers(const TrigMonROB* transObj,
		   TrigMonROB_p1* persObj,
		   MsgStream &log);

 private:

  T_AthenaPoolTPCnvStdVector<std::vector<TrigMonROBData>,
                             std::vector<TrigMonROBData_p1>, TrigMonROBDataCnv_p1> m_dataCnv;
};

#endif

