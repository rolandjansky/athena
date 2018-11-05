/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_HLTRESULMTMAKERALG_H
#define TRIGOUTPUTHANDLING_HLTRESULMTMAKERALG_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "TrigOutputHandling/HLTResultMTMakerTool.h"



/**
 * @class HLTResultMTMakerAlg 
 * @brief creates HLTResult and calls tools to fill the content,
 * this is offline tester alg as notmally the same operation will be performed by HLTMPPU component
 **/
class HLTResultMTMakerAlg : public AthReentrantAlgorithm {
public:
  HLTResultMTMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HLTResultMTMakerAlg() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute_r(const EventContext& context) const override;
  virtual StatusCode finalize() override;

private:
  SG::WriteHandleKey<HLT::HLTResultMT> m_resultKey { this, "ResultKey", "HLTResultMT", "Key of the output object"  };
  ToolHandleArray<HLTResultMTMakerTool> m_makerTools{ this, "MakerTools", {}, "set of tools that fill content of the HLTResultMT" };
  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitor" };
  
};

#endif // TRIGOUTPUTHANDLING_HLTRESULMTMAKERALG_H
