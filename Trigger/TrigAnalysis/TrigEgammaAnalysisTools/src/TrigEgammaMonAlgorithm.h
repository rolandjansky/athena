/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMAMONITORALGORITHM_H
#define TRIGEGAMMAANALYSISTOOLS_TRIGEGAMMAMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
class ITrigEgammaAnalysisBaseTool;

class TrigEgammaMonAlgorithm : public AthMonitorAlgorithm {
  public:
    TrigEgammaMonAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~TrigEgammaMonAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;


  private:

    std::vector<std::string> m_asgToolNames;
    ToolHandleArray< ITrigEgammaAnalysisBaseTool > m_asgtools;
};
#endif

