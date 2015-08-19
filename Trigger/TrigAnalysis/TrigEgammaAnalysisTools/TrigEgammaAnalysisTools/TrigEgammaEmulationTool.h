/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaEmulationTool_H
#define TrigEgammaEmulationTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavBaseTool.h"

class TrigEgammaEmulationTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaEmulationTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaEmulationTool( const std::string& myname );
  ~TrigEgammaEmulationTool() {};

  StatusCode childInitialize();
  StatusCode childBook();
  StatusCode childExecute();
  StatusCode childFinalize();
protected:
  StatusCode PrintL1FAILURE(const std::string, bool, bool);
  StatusCode TDCounts();
  bool EmulationEF(const std::string);
  bool EmulationPhotonEF(const std::string);
  bool EmulationL1(const std::string);
  bool EmulationL1V(const std::string,const float,const float);
  bool ApplyElectronPid(const xAOD::Electron *,const std::string);
  bool ApplyElectronCaloPid(const xAOD::Electron *,const std::string);
  /*! Directory name for each algorithm */
  std::string m_dir;

private:
  const xAOD::ElectronContainer *m_offElectrons;
  const xAOD::ElectronContainer *m_onlElectrons;
  const xAOD::PhotonContainer *m_onlPhotons;
  const xAOD::TrigElectronContainer *m_trigElectrons;
  const xAOD::CaloClusterContainer *m_caloClusters;
  const xAOD::TrigEMClusterContainer *m_trigEMClusters;
  const xAOD::EmTauRoIContainer *m_emTauRoI;
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronOnlIsEMTool;
  ToolHandleArray<IAsgPhotonIsEMSelector> m_photonOnlIsEMTool;
  ToolHandleArray<IAsgElectronLikelihoodTool> m_electronOnlLHTool;
  ToolHandleArray<IAsgElectronIsEMSelector> m_electronCaloIsEMTool;
  ToolHandleArray<IAsgPhotonIsEMSelector> m_photonCaloIsEMTool;
  ToolHandleArray<IAsgElectronLikelihoodTool> m_electronCaloLHTool;
  
  std::map<std::string,int> m_counterBits;
  std::map<std::string,int> m_counterBitsL1;
  std::map<std::string,int> m_counterEmulationHLT;
  std::map<std::string,int> m_counterEmulationL1;
  int m_eventCounter;
  /*! List of triggers from menu */
  std::vector<std::string> m_trigInputList;
 /*! List of trigger categories for MaM */
  std::vector<std::string> m_categories; 
  /*! List of triggers to study */
  std::vector<std::string> m_trigList; 
};

#endif
