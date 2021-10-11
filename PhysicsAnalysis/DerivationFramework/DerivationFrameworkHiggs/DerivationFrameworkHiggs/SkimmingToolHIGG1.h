/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolHIGG1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLHSG1_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLHSG1_H
 
#include <string>
#include <vector>
#include <algorithm>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

// xAOD header files
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEventInfo/EventInfo.h"


#include "TrigDecisionTool/TrigDecisionTool.h"

class IAsgElectronIsEMSelector;

namespace DerivationFramework {

  /** @class SkimmingToolHIGG1
      @author jsaxon@cern.ch 
      @author magdac@cern.ch
     */
 
  
 
 
  class SkimmingToolHIGG1 : public AthAlgTool, public ISkimmingTool {
   


    public: 
      /** Constructor with parameters */
      SkimmingToolHIGG1( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~SkimmingToolHIGG1();

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();

      /** Check that the current event passes this filter */
      virtual bool eventPassesFilter() const;

    private:

      ///////////////
      ///// TOOLS 

      ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
      


      // CUTS TO APPLY OR NOT
      bool m_reqGRL;
      bool m_reqLArError;
      bool m_reqTrigger;
      bool m_reqPreselection;
      bool m_incMergedElectron;
      bool m_incSingleElectron;
      bool m_incDoubleElectron;
      bool m_incSingleMuon;
      bool m_incDoubleMuon;          
      bool m_incDoubleElectronPhoton;
      bool m_incMergedElectronPhoton;
      bool m_incHighPtElectronPhoton;
      bool m_incTwoPhotons;
      bool m_reqKinematic;
      bool m_reqQuality;
      bool m_reqIsolation;
      bool m_reqInvariantMass;


      // CUT VALUES/SETTINGS

      std::string m_goodRunList;

      std::string m_defaultTrigger;
      std::vector<std::string> m_triggers;
      std::vector<std::string> m_mergedtriggers;

      mutable bool m_isMC;

      double m_minPhotonPt;
      bool   m_removeCrack;
      double m_maxEta;

      bool   m_relativePtCuts;
      double m_leadingPhotonPt;
      double m_subleadingPhotonPt;

      double m_minInvariantMass;
      double m_maxInvariantMass;

      double m_minElectronPt;
      double m_minMergedElectronPt;
      double m_minMuonPt;
      double m_maxMuonEta;
     

      ////////////////
      ///// FUNCTIONS

      // Cuts
      bool   SubcutOneMergedElectron() const;
      bool   SubcutGoodRunList() const;
      bool   SubcutLArError() const;
      bool   SubcutTrigger() const;
      bool   SubcutPreselect() const;
      bool   SubcutOnePhotonOneElectron() const;
      bool   SubcutTwoElectrons() const;
      bool   SubcutOnePhotonOneMuon() const;
      bool   SubcutOnePhotonTwoElectrons() const;
      bool   SubcutOnePhotonTwoMuons() const;
      bool   SubcutOnePhotonMergedElectrons() const;
      bool   SubcutHighPtOnePhotonOneElectron() const;

      bool   SubcutKinematic() const;
      bool   SubcutQuality() const;
      bool   SubcutIsolation() const;
      bool   SubcutInvariantMass() const;

      // Calculators
      bool   PhotonPreselect(const xAOD::Photon *ph) const;
      bool   ElectronPreselect(const xAOD::Electron *el) const;
      bool   MergedElectronPreselect(const xAOD::Electron *el) const;
      bool   MuonPreselect(const xAOD::Muon *mu) const;
      void   CalculateInvariantMass() const;
      void   GetDiphotonVertex() const;
      static double CorrectedEnergy(const xAOD::Photon *ph) ;
      double CorrectedEta(const xAOD::Photon *ph) const;
      static double ReturnRZ_1stSampling_cscopt2(double eta1) ;
    

      ///////////////
      ///// COUNTERS

      mutable unsigned int m_n_tot;
      mutable unsigned int m_n_passGRL;
      mutable unsigned int m_n_passLArError;
      mutable unsigned int m_n_passTrigger;
      mutable unsigned int m_n_passPreselect;
      mutable unsigned int m_n_passSingleElectronPreselect;
      mutable unsigned int m_n_passDoubleElectronPreselect;
      mutable unsigned int m_n_passSingleMuonPreselect;
      mutable unsigned int m_n_passSinglePhotonDoubleMuonPreselect;
      mutable unsigned int m_n_passSinglePhotonDoubleElectronPreselect;
      mutable unsigned int m_n_passSinglePhotonMergedElectronPreselect;
      mutable unsigned int m_n_passHighPtPhotonMergedElectronPreselect;
      mutable unsigned int m_n_passSingleMergedElectronPreselect;
      mutable unsigned int m_n_passKinematic;
      mutable unsigned int m_n_passQuality;
      mutable unsigned int m_n_passIsolation;
      mutable unsigned int m_n_passInvariantMass;
      mutable unsigned int m_n_pass;


      /////////////////////////////
      ///// EVENT LEVEL QUANTITIES

      mutable std::vector<const xAOD::Photon*> m_e_leadingPhotons;


      mutable bool m_e_passGRL;
      mutable bool m_e_passLArError;
      mutable bool m_e_passTrigger;
      mutable bool m_e_passPreselect;
      mutable bool m_e_passSingleElectronPreselect;
      mutable bool m_e_passDoubleElectronPreselect;
      mutable bool m_e_passSingleMuonPreselect;
      mutable bool m_e_passKinematic;
      mutable bool m_e_passQuality;
      mutable bool m_e_passIsolation;
      mutable bool m_e_passInvariantMass;

      mutable int    m_ph_pos_lead, m_ph_pos_subl;
      mutable double m_ph_pt_lead, m_ph_eta_lead, m_ph_phi_lead, m_ph_e_lead;
      mutable double m_ph_pt_subl, m_ph_eta_subl, m_ph_phi_subl, m_ph_e_subl;

      mutable int    m_ph_tight_lead, m_ph_tight_subl;

      mutable double m_e_invariantMass;
      mutable double m_e_diphotonZ;

      /////////////////////////////
      ///// FUNCTIONS

      static const double s_MZ;

      ////////////////////////////
      ///// TOOLS

      ToolHandle<IAsgElectronIsEMSelector> m_mergedCutTools;

      SG::ReadHandleKey<xAOD::EventInfo> 
        m_eventInfoKey { this, "EventInfoKey", "EventInfo", "" };

      SG::ReadHandleKey<xAOD::PhotonContainer >
        m_photonKey { this, "PhotonKey", "Photons", "" };

      SG::ReadHandleKey<xAOD::ElectronContainer >
        m_electronKey { this, "ElectronKey", "Electrons", "" };

      SG::ReadHandleKey<xAOD::MuonContainer >
        m_muonKey { this, "MuonKey", "Muons", "" };

  }; 

}

#endif // DERIVATIONFRAMEWORK_SKIMMINGTOOLEXAMPLE_H
