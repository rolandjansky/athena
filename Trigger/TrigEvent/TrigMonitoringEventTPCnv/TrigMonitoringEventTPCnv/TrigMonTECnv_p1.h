/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_TECNV_P1_H
#define TRIGMON_TECNV_P1_H

#include "TrigMonitoringEvent/TrigMonTE.h"
#include "TrigMonitoringEventTPCnv/TrigMonTE_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigMonTECnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigMonTE, TrigMonTE_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigMonTECnv_p1() {}
  virtual ~TrigMonTECnv_p1() {}

  virtual
  void persToTrans(const TrigMonTE_p1* persObj,
		   TrigMonTE* transObj,
		   MsgStream &log) const override;
  
  virtual
  void transToPers(const TrigMonTE* transObj,
		   TrigMonTE_p1* persObj,
		   MsgStream &log) const override;
};

#endif

