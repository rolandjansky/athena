/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTACTIVECONDALG_H
#define TRTACTIVECONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ToolHandle.h"
#include "TRT_ConditionsData/ActiveFraction.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"
#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

class TRT_ID;

//* Fills a eta,phi binned map of the fraction of straws, and posts it on CondStore 
class TRTActiveCondAlg : public AthAlgorithm
{
 public:
  typedef TRTCond::StrawStatusMultChanContainer StrawStatusContainer ;
  TRTActiveCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TRTActiveCondAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  ServiceHandle<ICondSvc> m_condSvc;
  SG::ReadCondHandleKey<StrawStatusContainer> m_strawReadKey{this,"StrawReadKey","/TRT/Cond/Status","Straw Status in-key"};
  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer for TRT"};

  SG::WriteCondHandleKey<TRTCond::ActiveFraction> m_strawWriteKey{this,"ActiveWriteKey","ActiveFraction","ActiveFraction out-key"};

  ToolHandle<ITRT_StrawStatusSummaryTool> m_strawStatus;
  const TRT_ID *m_trtId;

};
#endif
