/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaNavZeeTPPerf_H
#define TrigEgammaNavZeeTPPerf_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"
//#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisHelpers.h"
class TrigEgammaNavZeeTPPerf
: public TrigEgammaNavZeeTPBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaNavZeeTPPerf, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaNavZeeTPPerf( const std::string& myname );
  ~TrigEgammaNavZeeTPPerf() {};

  StatusCode childInitialize();
  StatusCode childExecute();
  StatusCode childFinalize();
  StatusCode TDCounts();
  StatusCode Distributions(const xAOD::Electron*, const xAOD::Electron*);
  StatusCode SimpleEfficiency();
  bool isMatchHLT(const xAOD::Electron *eloff);

private:
  std::map<std::string,int> m_counterBits;
  int m_eventCounter;
  const xAOD::ElectronContainer *m_offElectrons;
  const xAOD::ElectronContainer *m_onlElectrons;
  const xAOD::TrigElectronContainer *m_trigElectrons;
  const xAOD::CaloClusterContainer *m_caloClusters;
  const xAOD::TrigEMClusterContainer *m_trigEMClusters;
  const xAOD::EmTauRoIContainer *m_emTauRoI;
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronOnlPPCutIDTool;
};

#endif
