/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTSTRAWCONDALG_H
#define TRTSTRAWCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "TRT_ConditionsData/AliveStraws.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"
#include "TRT_ConditionsData/StrawStatusSummary.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

class TRT_ID;

class TRTStrawCondAlg : public AthReentrantAlgorithm
{
 public:
  typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer;

  TRTStrawCondAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;

  int findArrayTotalIndex(const int det, const int lay) const;
  int findArrayLocalWheelIndex(const int det, const int lay) const;

 private:
  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer for TRT"};
  SG::ReadCondHandleKey<TRTCond::StrawStatusSummary> m_strawStatusSummaryKey{this, "StrawStatusSummarKey", "StrawStatusSummary", "Key of StrawStatusSummary"};
  SG::WriteCondHandleKey<TRTCond::AliveStraws> m_strawWriteKey{this,"StrawWriteKey","AliveStraws","AliveStraws out-key"};

  const TRT_ID *m_trtId{};
};
#endif
