/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_ALGCNV_P1_H
#define TRIGCONF_ALGCNV_P1_H

#include "TrigMonitoringEvent/TrigConfAlg.h"
#include "TrigMonitoringEventTPCnv/TrigConfAlg_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigConfAlgCnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigConfAlg, TrigConfAlg_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigConfAlgCnv_p1() {}
  virtual ~TrigConfAlgCnv_p1() {}

  virtual
  void persToTrans(const TrigConfAlg_p1* persObj,
		   TrigConfAlg* transObj,
		   MsgStream &log) const override;
  
  virtual
  void transToPers(const TrigConfAlg* transObj,
		   TrigConfAlg_p1* persObj,
		   MsgStream &log) const override;
};

#endif

