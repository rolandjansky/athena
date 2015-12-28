/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZEEVALIDATION_ZEEVALIDATIONMONITORINGTOOL_H
#define ZEEVALIDATION_ZEEVALIDATIONMONITORINGTOOL_H

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "ReconElectronsPlots.h"
#include "TrueElectronsPlots.h"
#include "TrueFwdElectronsPlots.h"
#include "ZeePlots.h"
#include "FWDZeePlots.h"
	
// Root includes
#include "TH1.h"

// xAOD
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTruth/TruthParticle.h"

// Forward declaration
namespace ZeeValidation {
  
  class ZeeValidationMonitoringTool
    : public ManagedMonitorToolBase
  { 
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public: 
    
    // Copy constructor:
    
    // Constructor with parameters:
    ZeeValidationMonitoringTool( const std::string& type,
			 const std::string& name, 
			 const IInterface* parent );
    
    // Destructor:
    virtual ~ZeeValidationMonitoringTool(); 
    
    // Athena algtool's Hooks
    virtual StatusCode initialize();
    virtual StatusCode bookHistograms();
    virtual StatusCode fillHistograms();
    virtual StatusCode procHistograms();
        
    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////
    	
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
  private: 
    
    /// Default constructor:
    ZeeValidationMonitoringTool();
 
    // Protect against Truth in Data
    bool m_isData;

    // Containers
    std::string m_eventInfoName;
    std::string m_elecName; 
    std::string m_elecFwdName; 
    std::string m_photonName; 
    std::string m_vertexName;
    std::string m_trackName;
    std::string m_truthName;
    
    // Cut Values
    double m_PtCentCut, m_PtFwdCut;
    double m_MeeLowCut, m_MeeHighCut;
    double m_EtaCentCut, m_EtaLowFwdCut, m_EtaHighFwdCut;
    double m_EtaCrackLowCut, m_EtaCrackHighCut;
    double m_dRminRecoTrue;

    // Hists
    ZeeValidation::ReconElectronsPlots m_ReconElectronsPlots;
    ZeeValidation::TrueElectronsPlots m_TrueElectronsPlots;
    ZeeValidation::TrueFwdElectronsPlots m_TrueFwdElectronsPlots;
    ZeeValidation::ZeePlots m_ZeePlots;
    ZeeValidation::FWDZeePlots m_FWDZeePlots;

    // Matching True-Reco
    void MatchElec(const xAOD::TruthParticle* truth_electron, const xAOD::ElectronContainer* electrons);
    double drmin_elreco_truth;
    bool matchedE;
    const xAOD::Electron* matched_electron;

    void MatchPhot(const xAOD::TruthParticle* truth_electron, const xAOD::PhotonContainer* photons);
    double drmin_phreco_truth;
    bool matchedP;
    
    StatusCode book(PlotBase& plots);
  }; 
  
  // I/O operators
  //////////////////////
  
  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
}

#endif //> !ZEEVALIDATION_ZEEVALIDATIONMONITORINGTOOL_H
