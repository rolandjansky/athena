/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTSTRAWSTATUSCONDALG_H
#define TRTSTRAWSTATUSCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "TRT_ConditionsData/TRTStrawStatusData.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class TRT_ID;

class TRTStrawStatusCondAlg : public AthAlgorithm
{
 public:
  typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer ;
  TRTStrawStatusCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTStrawStatusCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  SG::ReadCondHandleKey<StrawStatusContainer> m_strawReadKey{this,"StrawReadKey","/TRT/Cond/Status","Straw Status in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_strawPermReadKey{this,"StrawPermanentReadKey","/TRT/Cond/StatusPermanent","Straw Permanent Status in-key"};

  SG::WriteHandleKey<TRTStrawStatusData>  m_strawStatusWriteKey{this,"StrawStatusWriteKey","StrawStatusData","Make StrawStatus available under this key"};
  SG::WriteHandleKey<TRTStrawStatusData>  m_strawStatusPermWriteKey{this,"StrawStatusPermWriteKey","StrawStatusPermanentData","Make StrawStatusPermanent Container under this key"};

  const TRT_ID* m_trtId{nullptr};
};
#endif
