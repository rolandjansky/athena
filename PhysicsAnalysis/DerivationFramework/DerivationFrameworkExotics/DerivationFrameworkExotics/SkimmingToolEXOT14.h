/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SkimmingToolEXOT14.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_SKIMMINGTOOLEXOT14_H
#define DERIVATIONFRAMEWORK_SKIMMINGTOOLEXOT14_H 1
 
#include <string>
#include <vector>
#include <algorithm>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/ISkimmingTool.h"

// #include "xAODRootAccess/TStore.h"

// xAOD header files
#include "xAODJet/JetContainer.h"
// #include "xAODEgamma/ElectronContainer.h"
// #include "xAODEgamma/PhotonContainer.h"
// #include "xAODMuon/MuonContainer.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "JetCalibTools/IJetCalibrationTool.h"

class JetCalibrationTool;

namespace DerivationFramework {

  /** @class SkimmingToolEXOT14
      @author jsaxon@cern.ch (adapted from Susumu Oda)
     */
  class SkimmingToolEXOT14 : public AthAlgTool, public ISkimmingTool {

    public: 
      /** Constructor with parameters */
      SkimmingToolEXOT14( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~SkimmingToolEXOT14();

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();

      /** Check that the current event passes this filter */
      virtual bool eventPassesFilter() const;

    private:

      ///////////////
      ///// TOOLS 

      ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
      ToolHandle<IJetCalibrationTool>    m_JESTool;

      ///////////////
      ///// SETTINGS

      std::string m_jetSGKey;

      // CUTS TO APPLY OR NOT
      bool m_reqGRL;
      bool m_reqLArError;
      bool m_reqTrigger;
      bool m_reqPreselection;
      bool m_reqJetPts;
      bool m_reqJetsDEta;
      bool m_reqDiJetMass;
      bool m_reqJetsDPhi;

      // CUT VALUES/SETTINGS

      mutable bool m_isMC;

      std::string m_goodRunList;

      std::string m_defaultTrigger;
      std::vector<std::string> m_triggers;

      double m_minJetPt;
      double m_maxEta;
      double m_leadingJetPt   ;
      double m_subleadingJetPt;
      double m_etaSeparation;
      double m_dijetMass;
      double m_jetDPhi;

      ////////////////
      ///// FUNCTIONS

      // Cuts
      bool   SubcutGoodRunList() const;
      bool   SubcutLArError() const;
      bool   SubcutTrigger() const;
      bool   SubcutPreselect() const;
      bool   SubcutJetPts() const;
      bool   SubcutJetDEta() const;
      bool   SubcutDijetMass() const;
      bool   SubcutJetDPhi() const;


      // Calculators
      void   JetPreselect(const xAOD::Jet *jet);

      // Helpers
      std::string TriggerVarName(std::string s) const;


      ///////////////
      ///// COUNTERS
      mutable unsigned int m_n_tot;
      mutable unsigned int m_n_passGRL;
      mutable unsigned int m_n_passLArError;
      mutable unsigned int m_n_passTrigger;
      mutable unsigned int m_n_passPreselect;
      mutable unsigned int m_n_passJetPts;
      mutable unsigned int m_n_passJetsDEta;
      mutable unsigned int m_n_passDiJetMass;
      mutable unsigned int m_n_passJetsDPhi;
      mutable unsigned int m_n_pass;


      /////////////////////////////
      ///// EVENT LEVEL QUANTITIES

      mutable TLorentzVector m_j1TLV, m_j2TLV;

      mutable bool m_e_passGRL;
      mutable bool m_e_passLArError;
      mutable bool m_e_passTrigger;
      mutable bool m_e_passPreselect;
      mutable bool m_e_passJetPts;
      mutable bool m_e_passJetsDEta;
      mutable bool m_e_passDiJetMass;
      mutable bool m_e_passJetsDPhi;

      mutable double m_e_JetsDEta;
      mutable double m_e_DiJetMass;
      mutable double m_e_JetsDPhi;

      /////////////////////////////
      ///// FUNCTIONS

      static const double s_MZ;

  }; 

}

#endif // DERIVATIONFRAMEWORK_SKIMMINGTOOLEXAMPLE_H
