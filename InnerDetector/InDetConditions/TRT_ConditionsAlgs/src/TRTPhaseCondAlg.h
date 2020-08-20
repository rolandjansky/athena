/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTPHASECONDALG_H
#define TRTPHASECONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "TRT_ConditionsData/AverageT0.h"
#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class TRT_ID;

class TRTPhaseCondAlg : public AthAlgorithm
{
 public:
  typedef TRTCond::StrawT0MultChanContainer StrawT0Container ;
  TRTPhaseCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTPhaseCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;


 private:
  ServiceHandle<ICondSvc> m_condSvc;
  SG::ReadCondHandleKey<StrawT0Container> m_T0ReadKey{this,"T0ReadKey","/TRT/Calib/T0","T0 in-key"};
  SG::WriteCondHandleKey<TRTCond::AverageT0> m_T0WriteKey{this,"T0WriteKey","AverageT0","Average T0 out-key"};
  ToolHandle<ITRT_CalDbTool> m_caldbtool;
  const TRT_ID *m_trtId;

};
#endif
