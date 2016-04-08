///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TopPhysValMonitoring.h
// Header file for class TopPhysValMonitoring
// Author: Markus Cristinziani <cristinz@uni-bonn.de>
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

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Root includes
#include "TH1.h"

// Forward declaration

namespace TopPhysVal {

  class TopPhysValMonitoring : public ManagedMonitorToolBase {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
    public:

      // Copy constructor:

      /// Constructor with parameters:
      TopPhysValMonitoring( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

      /// Destructor:
      virtual ~TopPhysValMonitoring();

      // Athena algtool's Hooks
      virtual StatusCode initialize();
      virtual StatusCode bookHistograms();
      virtual StatusCode fillHistograms();
      virtual StatusCode procHistograms();

      // own methods
      StatusCode retrieveObjects();
      StatusCode fillBasicHistograms();
  
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
      TopPhysValMonitoring();

      // configuration: container names
      std::string m_c_jetName;
      std::string m_c_elecName;
      std::string m_c_photonName;
      std::string m_c_muonName;
      std::string m_c_tauName;
      std::string m_c_metName;
      std::string m_c_metContainerName;
      std::string m_c_histBaseDirectory;             ///< root directory for all plots produced by this module (has to end in "/" if not empty)

      // containers
      const xAOD::JetContainer*      m_jets;
      const xAOD::ElectronContainer* m_elecs;
      const xAOD::PhotonContainer*   m_photons;
      const xAOD::MuonContainer*     m_muons;
      const xAOD::TauJetContainer*   m_taus;
      
      
      // selected objects
      std::vector<uint> m_idx_elec_baseline, m_idx_elec_signal;
      std::vector<uint> m_idx_muon_baseline, m_idx_muon_signal;
      
      // Hists
      TH1D* h_jetPt;
      TH1D* h_elecPt;
      TH1D* h_photonPt;
      TH1D* h_muonPt;
      TH1D* h_tauPt;
      TH1D* h_tauEta;
      TH1D* h_metEt;
      TH1D* h_NJets;

    };

    // I/O operators
    //////////////////////

    ///////////////////////////////////////////////////////////////////
    // Inline methods:
    ///////////////////////////////////////////////////////////////////
}

#endif //> !PHYSVALMONITORING_PHYSVALEXAMPLE_H
