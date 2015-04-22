// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaValidationTool_H
#define TrigEgammaValidationTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"
class TrigEgammaValidationTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaValidationTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaValidationTool( const std::string& myname );
  ~TrigEgammaValidationTool() {};

  StatusCode childInitialize();
  StatusCode childExecute();
  StatusCode childFinalize();
protected:

private:
  int m_eventCounter;
  /*! Directory name for each algorithm */
  std::string m_dir;
  /*! List of triggers to study */
  std::vector<std::string> m_probeTrigList;
  /* Offline Photon Pid */
  std::string m_photonPid;
  /* Use only unconverted photons */
  bool m_doUnconverted;
  
  const xAOD::ElectronContainer *m_offElectrons;
  const xAOD::PhotonContainer *m_offPhotons;
  const xAOD::ElectronContainer *m_onlElectrons;
  const xAOD::PhotonContainer *m_onlPhotons;
  const xAOD::TrigElectronContainer *m_trigElectrons;
  const xAOD::CaloClusterContainer *m_caloClusters;
  const xAOD::TrigEMClusterContainer *m_trigEMClusters;
  const xAOD::EmTauRoIContainer *m_emTauRoI;
  void fillTracking(const xAOD::Electron*, const xAOD::Electron*);
  void fillShowerShapes(const xAOD::Egamma*, const xAOD::Egamma*);
  void SimpleElectronEfficiency(const std::string,const float,const std::string);
  void SimplePhotonEfficiency(const std::string,const float);
  void SimpleEfficiency(const std::string);
  void fillRes(const xAOD::Egamma *eloff, const xAOD::Egamma *eg);
};

#endif
