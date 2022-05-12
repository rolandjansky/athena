/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTSTRAWSTATUSCONDALG_H
#define TRTSTRAWSTATUSCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ConditionsData/StrawStatusData.h"
#include "TRT_ConditionsData/StrawStatusSummary.h"

class TRT_ID;

class TRTStrawStatusCondAlg : public AthReentrantAlgorithm
{
 public:
  typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer;

  TRTStrawStatusCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTStrawStatusCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

 private:
  SG::ReadCondHandleKey<StrawStatusContainer> m_strawReadKey{this,"StrawReadKey","/TRT/Cond/Status","Straw Status in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_strawPermReadKey{this,"StrawPermanentReadKey","/TRT/Cond/StatusPermanent","Straw Permanent Status in-key"};
  SG::ReadCondHandleKey<StrawStatusContainer> m_strawHTReadKey{this,"StatHTReadKeyName","/TRT/Cond/StatusHT","StrawStatusHT in-key"};

  SG::WriteCondHandleKey<TRTCond::StrawStatusData> m_strawStatusWriteKey{this,"StrawStatusWriteKey","StrawStatusData","Make StrawStatus available under this key"};
  SG::WriteCondHandleKey<TRTCond::StrawStatusData> m_strawStatusPermWriteKey{this,"StrawStatusPermWriteKey","StrawStatusPermanentData","Make StrawStatusPermanent Container under this key"};
  SG::WriteCondHandleKey<TRTCond::StrawStatusData> m_strawStatusHTWriteKey{this,"StrawStatusHTWriteKey","StrawStatusHTData","Make StrawStatusHT Container under this key"};
  SG::WriteCondHandleKey<TRTCond::StrawStatusSummary> m_strawStatusSummaryWriteKey{this,"StrawStatusSummaryWriteKey","StrawStatusSummary","Make StrawStatusSummary available under this key"};
  SG::WriteCondHandleKey<TRTCond::StrawStatusSummary> m_strawStatusHTSummaryWriteKey{this,"StrawStatusHTSummaryWriteKey","StrawStatusHTSummary","Make StrawStatusHTSummary available under this key"};

  const TRT_ID* m_trtId{nullptr};
};
#endif
