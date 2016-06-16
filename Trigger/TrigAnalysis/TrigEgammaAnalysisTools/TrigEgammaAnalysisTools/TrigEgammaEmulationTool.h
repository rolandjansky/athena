/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaEmulationTool_H
#define TrigEgammaEmulationTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaNavBaseTool.h"

// ******* For track isolation ********************************************
#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationCorrection.h"
#include "RecoToolInterfaces/IsolationCommon.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
// ************************************************************************

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
  StatusCode TDCounts();
  bool EmulationEF(const std::string);
  bool EmulationEF_Perf(const std::string);
  bool EmulationPhotonEF(const std::string);
  bool EmulationL1(const std::string);
  bool IsolationL1(const float, const float, const float, const float, const float);
  bool VariableEtL1(const std::string,const float, const float);
  // bool EmulationL1V(const std::string,const float,const float);
  float EmulationL1V(const std::string,const float);
  bool ApplyElectronPid(const xAOD::Electron *,const std::string);
  bool ApplyElectronCaloPid(const xAOD::Electron *,const std::string);
  // Track Isolation
  // double TrackIsolation(const xAOD::Electron *, const xAOD::TrackParticleContainer);
  void TrackIsolation(); // original version
  // 
  void InitializeTriggerList(int);
  void InitializeMaps();
  void fillL1histograms(const std::vector<xAOD::EmTauRoI*>,const std::vector<xAOD::EmTauRoI*>,const std::string);
  void PrintDebugInfo(const std::string,const std::string,const float,const float,const std::string,const std::string,const bool,const bool,const bool,const bool,const bool);
  std::string getThisL1Item(std::string);
  /*! Directory name for each algorithm */
  std::string m_dir;
  std::string m_dir_tdcounts;
  std::map<std::string,std::string> mapL1Directory;
  std::map<std::string,std::string> mapHLTDirectory;
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
  // ******* For track isolation ********************************************
  ToolHandle<InDet::IInDetTrackSelectionTool> m_inDetTrackSelectionTool;
  ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
  // ************************************************************************
  std::map<std::string,int> m_counterBits;
  std::map<std::string,int> m_counterBitsL1;
  std::map<std::string,int> m_counterEmulationHLT;
  std::map<std::string,int> m_counterEmulationHLT_Perf;
  std::map<std::string,int> m_counterEmulationL1;

  int m_eventCounter;
  /*! List of triggers from menu */
  std::vector<std::string> m_trigInputList;
  std::vector<std::string> m_trigListElectrons;
  std::vector<std::string> m_trigListPhotons;
  std::vector<std::string> m_trigL1;
  /*! List of trigger categories for MaM */
  std::vector<std::string> m_categories; 
  /*! List of triggers to study */
  std::vector<std::string> m_trigList; 
  std::map<std::string,bool> mapL1TDTDec;
};

#endif
