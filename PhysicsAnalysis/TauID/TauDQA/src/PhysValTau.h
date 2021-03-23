// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
//
#ifndef TAUDQA_PHYSVALTAU_H
#define TAUDQA_PHYSVALTAU_H

// STL includes
#include <memory>
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "TauAnalysisTools/ITauTruthMatchingTool.h"
#include "TauAnalysisTools/ITauSelectionTool.h"

// Local includes
#include "TauValidationPlots.h"
#include "RecoTypes.h"

class PhysValTau
  : public ManagedMonitorToolBase
{ 

public: 
  /// Constructor with parameters: 
  PhysValTau( const std::string& type,
	      const std::string& name, 
	      const IInterface* parent );

  // Athena algtool's Hooks
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();


private: 
  // Containers
  std::string m_TauJetContainerName; 
  std::string m_TruthParticleContainerName; 

  bool m_isMC;           //!< Are we running over MC data?
    
  //Variable Definitions
    
  /*keeps track of matched tau jets*/    
  std::vector<size_t> m_matched_itr;

  // Tool used for truth-matching
  ToolHandle<TauAnalysisTools::ITauTruthMatchingTool> m_truthTool;
  // Tool used to select "primitive" and "nominal" taus
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_primTauSel;
  ToolHandle<TauAnalysisTools::ITauSelectionTool> m_nomiTauSel;

  //Histograms
  // general tau all prongs plots
  std::unique_ptr<TauValidationPlots> m_oTauValidationPlots;
  // TauValidationPlots m_oTauValidationPlots;
  bool matchTrueAndRecoTau (const xAOD::TauJetContainer *&taus, const xAOD::TruthParticle* trueTau, const xAOD::TauJet* &matchedRecoTau );
  
}; 

#endif //> !TAUDQA_PHYSVALTAU_H
