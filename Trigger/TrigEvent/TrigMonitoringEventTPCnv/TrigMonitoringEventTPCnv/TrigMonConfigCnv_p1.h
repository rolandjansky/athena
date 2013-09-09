/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_CONFIGCNV_P1_H
#define TRIGMON_CONFIGCNV_P1_H

// Framework
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Transient data
#include "TrigMonitoringEvent/TrigMonConfig.h"

// Persistent data and converters
#include "TrigMonitoringEventTPCnv/TrigMonConfig_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfChainCnv_p1.h"
#include "TrigMonitoringEventTPCnv/TrigConfSeqCnv_p1.h"

class MsgStream;

class TrigMonConfigCnv_p1 : public T_AthenaPoolTPCnvBase<TrigMonConfig, TrigMonConfig_p1>
{
 public:
  
  TrigMonConfigCnv_p1() {}
  virtual ~TrigMonConfigCnv_p1() {}
  
  void persToTrans(const TrigMonConfig_p1* persObj,
		   TrigMonConfig* transObj,
		   MsgStream &log);
  
  void transToPers(const TrigMonConfig* transObj,
		   TrigMonConfig_p1* persObj,
		   MsgStream &log);

 private:

  T_AthenaPoolTPCnvStdVector<std::vector<TrigConfChain>, 
                             std::vector<TrigConfChain_p1>, TrigConfChainCnv_p1> m_chainCnv;

  T_AthenaPoolTPCnvStdVector<std::vector<TrigConfSeq>,
                             std::vector<TrigConfSeq_p1>, TrigConfSeqCnv_p1> m_seqCnv;
};

#endif

