/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISTOP_TOPEVENTRECONSTRCUTIONTOOLS_PSEUDOTOPRECO_H

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "TopParticleLevel/ParticleLevelEvent.h"

// ROOT include(s):
#include "TLorentzVector.h"

// system include(s):
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <memory>


  ///
  /// @translated by A.Knue from MAMbo tool, Link: https://twiki.cern.ch/twiki/bin/view/Main/MAMbo
  ///
  /// @brief use PseudoTop definitions to reconstruct tttbar event
  ///  
  
namespace PseudoTopReco{
  class Particles;
}

namespace top{
  class Event;
  class TopConfig;
  
  enum LeptonType{kElectron, kMuon};

  class PseudoTopReco final : public asg::AsgTool {

    public:      
      /// Constructor
      explicit PseudoTopReco( const std::string& name );
      /// Destructor
      virtual ~PseudoTopReco(){}
      
      /// Function initialising the tool
      virtual StatusCode initialize();
      /// Function executing the tool
      virtual StatusCode execute(const top::Event&);

      virtual StatusCode execute(const top::ParticleLevelEvent& plEvent);

      /// Function finalizing the tool
      virtual StatusCode finalize();
      
      // fill first all four vectors to use for the reconstruction
      bool SetChargedLeptonInfo(const top::Event& event);
      bool SetJetInfo(const top::Event& event);

      bool SetChargedLeptonInfo(const top::ParticleLevelEvent& plEvent);
      bool SetJetInfo(const top::ParticleLevelEvent& plEvent);

      bool RunReconstruction();

      bool ReconstructLeptonicW();

    private:
      
      // config file
      std::shared_ptr<top::TopConfig> m_config;
      float m_bTagCutValue;      

      std::string m_leptonType;

      std::vector<TLorentzVector> m_bJets;
      std::vector<TLorentzVector> m_lightJets;
      TLorentzVector m_lepton;
      TLorentzVector m_neutrino;
      TLorentzVector m_W_lep;
      TLorentzVector m_W_had;
      TLorentzVector m_b_lep;
      TLorentzVector m_b_had;
      TLorentzVector m_top_lep;
      TLorentzVector m_top_had;
      TLorentzVector m_ttbar;
      double m_nu_px, m_nu_py, m_nu_pz, m_met_et;
      double m_ht,m_R_lb,m_R_Wb_had,m_R_Wb_lep;

      const double KinemEdge = 13.9e6;
      const double mWPDG     =  80.399;

    
  };
}
#endif
