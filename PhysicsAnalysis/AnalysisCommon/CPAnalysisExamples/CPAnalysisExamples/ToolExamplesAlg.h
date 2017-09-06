/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CPANALYSISEXAMPLES_TOOLEXAMPLESALG_H
#define CPANALYSISEXAMPLES_TOOLEXAMPLESALG_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

///General
#include "AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h"
#include "PileupReweighting/PileupReweightingTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"
#include "AssociationUtils/IOverlapRemovalTool.h"

///Electrons
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"
#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "PATCore/IAsgSelectionTool.h"

#include "IsolationSelection/IIsolationSelectionTool.h"
#include "IsolationSelection/IIsolationCloseByCorrectionTool.h"

#include "ElectronEfficiencyCorrection/IAsgElectronEfficiencyCorrectionTool.h"

///Muons
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"

#include "MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h"


///Jets
#include "JetCalibTools/IJetCalibrationTool.h"
#include "JetInterface/IJetUpdateJvt.h"
#include "JetInterface/IJetSelector.h"






class ToolExamplesAlg: public ::AthAnalysisAlgorithm { 
 public: 
  ToolExamplesAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~ToolExamplesAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  firstExecute();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private: 
 
  ///General
   ToolHandle<IGoodRunsListSelectionTool> m_grl;
   ToolHandle<CP::IPileupReweightingTool> m_prw;
   ToolHandle<Trig::TrigDecisionTool> m_tdt;
   ToolHandle<Trig::IMatchingTool> m_tmt;
   ToolHandle<ORUtils::IOverlapRemovalTool> m_or;
 
  ///Electrons
  ToolHandle<CP::IEgammaCalibrationAndSmearingTool> m_elCalib;
  ToolHandle<CP::IIsolationCorrectionTool> m_elIsoCalib;
  ToolHandle<IAsgSelectionTool> m_elSelection;
  ToolHandle<CP::IIsolationSelectionTool> m_elIsoSelection;
  ToolHandle<CP::IIsolationCloseByCorrectionTool> m_elCorrectedIsoSelection;
  
  ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> m_elSFTools;
  
  ///Muons
  ToolHandle<CP::IMuonCalibrationAndSmearingTool> m_muCalib;
  ToolHandle<CP::IMuonSelectionTool> m_muSelectionTool;
  ToolHandle<CP::IIsolationSelectionTool> m_muIsoSelection;
  
  ToolHandleArray<CP::IMuonEfficiencyScaleFactors> m_muSFTools;
  
  ///Jets
   ToolHandle<IJetCalibrationTool> m_jetCalib;
   ToolHandle<IJetSelector> m_jetCleaning;


}; 

#endif //> !CPANALYSISEXAMPLES_TOOLEXAMPLESALG_H
