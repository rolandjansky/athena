/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      
      ///////////////
      ///// SETTINGS

      std::string m_photonSGKey;
      std::string m_electronSGKey;
      std::string m_muonSGKey;

      // CUTS TO APPLY OR NOT
      bool m_reqGRL;
      bool m_reqLArError;
      bool m_reqTrigger;
      bool m_reqPreselection;
      bool m_incSingleElectron;
      bool m_incDoubleElectron;
      bool m_incSingleMuon;
      bool m_incDoubleMuon;          
      bool m_incDoubleElectronPhoton;
      bool m_incMergedElectron;
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
      double m_minMuonPt;
      double m_maxMuonEta;
     

      ////////////////
      ///// FUNCTIONS

      // Cuts
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
      double CorrectedEnergy(const xAOD::Photon *ph) const;
      double CorrectedEta(const xAOD::Photon *ph) const;
      double ReturnRZ_1stSampling_cscopt2(double eta1) const;
    

      ///////////////
      ///// COUNTERS

      mutable unsigned int n_tot;
      mutable unsigned int n_passGRL;
      mutable unsigned int n_passLArError;
      mutable unsigned int n_passTrigger;
      mutable unsigned int n_passPreselect;
      mutable unsigned int n_passSingleElectronPreselect;
      mutable unsigned int n_passSinglePhotonDoubleMuonPreselect;
      mutable unsigned int n_passSinglePhotonDoubleElectronPreselect;
      mutable unsigned int n_passSinglePhotonMergedElectronPreselect;
      mutable unsigned int n_passHighPtPhotonMergedElectronPreselect;
      mutable unsigned int n_passDoubleElectronPreselect;
      mutable unsigned int n_passSingleMuonPreselect;
      mutable unsigned int n_passKinematic;
      mutable unsigned int n_passQuality;
      mutable unsigned int n_passIsolation;
      mutable unsigned int n_passInvariantMass;
      mutable unsigned int n_pass;


      /////////////////////////////
      ///// EVENT LEVEL QUANTITIES

      mutable std::vector<const xAOD::Photon*> e_leadingPhotons;

  
      mutable TLorentzVector leadPhotonLV;
      mutable TLorentzVector sublPhotonLV;

      mutable bool e_tot;
      mutable bool e_passGRL;
      mutable bool e_passLArError;
      mutable bool e_passTrigger;
      mutable bool e_passPreselect;
      mutable bool e_passSingleElectronPreselect;
      mutable bool e_passDoubleElectronPreselect;
      mutable bool e_passSingleMuonPreselect;
      mutable bool e_passKinematic;
      mutable bool e_passQuality;
      mutable bool e_passIsolation;
      mutable bool e_passInvariantMass;
      mutable bool e_pass;

      mutable int    ph_pos_lead, ph_pos_subl;
      mutable double ph_pt_lead, ph_eta_lead, ph_phi_lead, ph_e_lead;
      mutable double ph_pt_subl, ph_eta_subl, ph_phi_subl, ph_e_subl;

      mutable int    ph_tight_lead, ph_tight_subl;
      mutable int    ph_iso_lead, ph_iso_subl;

      mutable double e_invariantMass;
      mutable double e_diphotonZ;

      /////////////////////////////
      ///// FUNCTIONS

      static const double s_MZ;

      ////////////////////////////
      ///// TOOLS

      ToolHandle<IAsgElectronIsEMSelector> m_mergedCutTools;

  }; 

}

#endif // DERIVATIONFRAMEWORK_SKIMMINGTOOLEXAMPLE_H
