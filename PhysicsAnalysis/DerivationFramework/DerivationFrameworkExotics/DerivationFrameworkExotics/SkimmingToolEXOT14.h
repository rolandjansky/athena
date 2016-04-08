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
      mutable unsigned int n_tot;
      mutable unsigned int n_passGRL;
      mutable unsigned int n_passLArError;
      mutable unsigned int n_passTrigger;
      mutable unsigned int n_passPreselect;
      mutable unsigned int n_passJetPts;
      mutable unsigned int n_passJetsDEta;
      mutable unsigned int n_passDiJetMass;
      mutable unsigned int n_passJetsDPhi;
      mutable unsigned int n_pass;


      /////////////////////////////
      ///// EVENT LEVEL QUANTITIES

      mutable TLorentzVector j1TLV, j2TLV;

      mutable bool e_passGRL;
      mutable bool e_passLArError;
      mutable bool e_passTrigger;
      mutable bool e_passPreselect;
      mutable bool e_passJetPts;
      mutable bool e_passJetsDEta;
      mutable bool e_passDiJetMass;
      mutable bool e_passJetsDPhi;
      mutable bool e_pass;

      mutable double e_JetsDEta;
      mutable double e_DiJetMass;
      mutable double e_JetsDPhi;

      /////////////////////////////
      ///// FUNCTIONS

      static const double s_MZ;

  }; 

}

#endif // DERIVATIONFRAMEWORK_SKIMMINGTOOLEXAMPLE_H
