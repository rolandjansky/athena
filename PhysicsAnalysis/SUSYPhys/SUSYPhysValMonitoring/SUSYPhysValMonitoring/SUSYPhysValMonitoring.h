///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SUSYPhysValMonitoring.h
// Header file for class SUSYPhysValMonitoring
// Author:
///////////////////////////////////////////////////////////////////
#ifndef PHYSVALMONITORING_PHYSVALEXAMPLE_H
#define PHYSVALMONITORING_PHYSVALEXAMPLE_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Root includes
#include "TH1.h"
#include "TDatabasePDG.h"

// Forward declaration

namespace SUSYPhysVal {

  class SUSYPhysValMonitoring : public ManagedMonitorToolBase {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    public:

      // Copy constructor:

      /// Constructor with parameters:
      SUSYPhysValMonitoring( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

      /// Destructor:
      virtual ~SUSYPhysValMonitoring();

      // Athena algtool's Hooks
      virtual StatusCode initialize();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();

      // own methods
      StatusCode retrieveObjects();
      StatusCode fillBasicHistograms();
      StatusCode fillTruthHistograms();
      double GetEventWeight();
  
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
      SUSYPhysValMonitoring();

      // configuration: container names
      std::string m_c_jetName;
      std::string m_c_elecName;
      std::string m_c_photonName;
      std::string m_c_muonName;
      std::string m_c_tauName;
      std::string m_c_pvxName;
      std::string m_c_truthName;
      std::string m_c_metName;
      std::string m_c_metContainerName;
      
      // configuration
      std::string m_c_histBaseDirectory;             ///< root directory for all plots produced by this module (has to end in "/" if not empty)
      bool m_c_use_truth_information;                ///< use truth information

      // containers
      const xAOD::JetContainer*      m_jets;
      const xAOD::ElectronContainer* m_elecs;
      const xAOD::PhotonContainer*   m_photons;
      const xAOD::MuonContainer*     m_muons;
      const xAOD::TauJetContainer*   m_taus;
//       const xAOD::VertexContainer*   m_pvx;
      const xAOD::TruthParticleContainer* m_truth;
      
      // selected objects
      std::vector<uint> m_idx_elec_baseline, m_idx_elec_signal;
      std::vector<uint> m_idx_muon_baseline, m_idx_muon_signal;
      
      // Histograms
      TH1D* h_jet_Pt;
      TH1D* h_jet_eta;
      TH1D* h_elec_Pt;
      TH1D* h_elec_eta;
      TH1D* h_photon_Pt;
      TH1D* h_muon_Pt;
      TH1D* h_muon_eta;
      TH1D* h_tau_Pt;
      TH1D* h_tau_eta;
      TH1D* h_met_Et;
      TH1D* h_met_phi;
      
      TH1D* h_SUSY_particle_IDs;
      TH1D* h_SUSY_particle_parentIDs;

  };

  
    // I/O operators
    //////////////////////

    ///////////////////////////////////////////////////////////////////
    // Inline methods:
    ///////////////////////////////////////////////////////////////////
    
    
}



#endif //> !PHYSVALMONITORING_PHYSVALEXAMPLE_H
