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
protected:
  StatusCode fillTracking(const xAOD::Electron*, const xAOD::Electron*);
  StatusCode fillShowerShapes(const xAOD::Egamma*, const xAOD::Egamma*);
  void SimpleElectronEfficiency(const std::string,const std::string);
  void SimplePhotonEfficiency(const std::string,const std::string);
  void SimpleEfficiency();
  bool isMatchHLT(const xAOD::Egamma*);
  void fillRes(const xAOD::Electron *eloff, const xAOD::Electron *eg);

private:
  int m_eventCounter;
  const xAOD::ElectronContainer *m_offElectrons;
  const xAOD::PhotonContainer *m_offPhotons;
  const xAOD::ElectronContainer *m_onlElectrons;
  const xAOD::PhotonContainer *m_onlPhotons;
  const xAOD::TrigElectronContainer *m_trigElectrons;
  const xAOD::CaloClusterContainer *m_caloClusters;
  const xAOD::TrigEMClusterContainer *m_trigEMClusters;
  const xAOD::EmTauRoIContainer *m_emTauRoI;
};

#endif
