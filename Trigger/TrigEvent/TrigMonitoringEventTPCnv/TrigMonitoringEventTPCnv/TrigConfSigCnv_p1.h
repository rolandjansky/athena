/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_SIGCNV_P1_H
#define TRIGCONF_SIGCNV_P1_H

#include "TrigMonitoringEvent/TrigConfSig.h"
#include "TrigMonitoringEventTPCnv/TrigConfSig_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigConfSigCnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigConfSig, TrigConfSig_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigConfSigCnv_p1() {}
  virtual ~TrigConfSigCnv_p1() {}

  virtual
  void persToTrans(const TrigConfSig_p1* persObj,
		   TrigConfSig* transObj,
		   MsgStream &log) const override;
  
  virtual
  void transToPers(const TrigConfSig* transObj,
		   TrigConfSig_p1* persObj,
		   MsgStream &log) const override;
};

#endif

