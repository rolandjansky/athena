/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONF_SEQCNV_P1_H
#define TRIGCONF_SEQCNV_P1_H

// Framework
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Transient data
#include "TrigMonitoringEvent/TrigConfSeq.h"

// Persistent data and converters
#include "TrigMonitoringEventTPCnv/TrigConfSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfAlgCnv_p1.h"

class MsgStream;

class TrigConfSeqCnv_p1 : public T_AthenaPoolTPCnvBase<TrigConfSeq, TrigConfSeq_p1>
{
 public:
  
  TrigConfSeqCnv_p1() {}
  virtual ~TrigConfSeqCnv_p1() {}
  
  void persToTrans(const TrigConfSeq_p1* persObj,
		   TrigConfSeq* transObj,
		   MsgStream &log);

  void transToPers(const TrigConfSeq* transObj,
		   TrigConfSeq_p1* persObj,
		   MsgStream &log);

 private:

  T_AthenaPoolTPCnvStdVector<std::vector<TrigConfAlg>, 
                             std::vector<TrigConfAlg_p1>, TrigConfAlgCnv_p1> m_algCnv;
};

#endif

