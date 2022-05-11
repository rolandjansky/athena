/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_boostedxbbtag_header
#define jetsubstructureutils_boostedxbbtag_header

// @Author: Giordon Stark
// @Email: gstark@cern.ch

// c++ includes
#include <set>
#include <string>
#include<memory>

// EDM includes
#include <xAODJet/JetContainer.h>
#include <xAODMuon/MuonContainer.h>
#include "MuonSelectorTools/MuonSelectionTool.h"

// forward-declare the ROOT includes
class TFile;
class TH2;

namespace JetSubStructureUtils {
  class BoostedXbbTag {
    public:
      // standard tool constructor
      BoostedXbbTag(std::string working_point           = "medium",
#ifdef ROOTCORE
                    std::string recommendations_file    = "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Htagging_MC15c_77WP_20160522.dat",
#else
                    std::string recommendations_file    = "JetSubStructureUtils/data/config_13TeV_Htagging_MC15c_77WP_20160522.dat",
#endif
                    std::string boson_type              = "Higgs",
                    std::string algorithm_name          = "AK10LCTRIMF5R20",
                    int num_bTags                       = 2,
                    std::string decor_prefix            = "",
                    bool debug                          = false,
                    bool verbose                        = false);

      // this is recommended usage, pass in jet, muons, get true/false
      int result(const xAOD::Jet& jet, const xAOD::MuonContainer* muons) const;
      // sometimes you don't have certain properties set so pass them in
      //    to select the appropriate tagging recommendation
      int result(const xAOD::Jet& jet, std::string algorithm_name, const xAOD::MuonContainer* muons) const;

      // given the jet and configurations, return the string representation of the jet
      //        eg: AK10LCTRIMF5R20, CA10LCPRUNR50Z15, CA12LCBDRSM100R30Y15
      std::pair<bool, std::string> get_algorithm_name(const xAOD::Jet& jet,
                                     const xAOD::JetAlgorithmType::ID jet_algorithm,
                                     const float size_parameter,
                                     const xAOD::JetInput::Type jet_input,
                                     const xAOD::JetTransform::Type jet_transform) const;

      // return a vector of track jets which are btagged by the tool
      std::vector<const xAOD::Jet*> get_bTagged_trackJets(const xAOD::Jet& jet) const;
      // return the matched muon object (nullptr if there is not one)
      std::vector<const xAOD::Muon*> get_matched_muons(const xAOD::Jet& jet) const;
      TLorentzVector get_correctedJet_TLV(const xAOD::Jet& jet) const;
      std::pair<float, float> get_mass_window(const xAOD::Jet& jet) const;
      std::pair<float, std::string> get_D2_pivot(const xAOD::Jet& jet) const;

    private:
      std::string m_working_point;
      std::string m_recommendations_file;
      std::string m_boson_type;
      std::string m_algorithm_name;
      int m_num_bTags;
      std::string m_decor_prefix;
      bool m_debug,
           m_verbose;

      float m_bTagCut,
            m_bTagCutCharm,
            m_massMin,
            m_massMax;
      std::vector<float> m_D2_params;
      std::string m_D2_cut_direction;
      std::unique_ptr<CP::MuonSelectionTool> m_muonSelectionTool;

      // this is so we don't error out in general, esp. for athena jobs
      bool m_bad_configuration;

      // main 4 details for classifying a jet
      static const SG::AuxElement::ConstAccessor<int> s_AlgorithmType;
      static const SG::AuxElement::ConstAccessor<float> s_SizeParameter;
      static const SG::AuxElement::ConstAccessor<int> s_InputType;
      static const SG::AuxElement::ConstAccessor<int> s_TransformType;

      // for trimming
      static const SG::AuxElement::ConstAccessor<float> s_RClus;
      static const SG::AuxElement::ConstAccessor<float> s_PtFrac;

      // for pruning
      static const SG::AuxElement::ConstAccessor<float> s_RCut;
      static const SG::AuxElement::ConstAccessor<float> s_ZCut;

      // for splitting
      // static const SG::AuxElement::ConstAccessor<int> NSubjetMax ("NSubjetMax");
      static const SG::AuxElement::ConstAccessor<char> s_BDRS;
      /* MuMax, YMin, RClus */
      // static const SG::AuxElement::ConstAccessor<float> RClus ("RClus"); // defined above for trimming
      static const SG::AuxElement::ConstAccessor<float> s_YMin;
      static const SG::AuxElement::ConstAccessor<float> s_MuMax;

      // for D2
      static const SG::AuxElement::ConstAccessor<float> s_D2;
      static const SG::AuxElement::ConstAccessor<float> s_ECF1;
      static const SG::AuxElement::ConstAccessor<float> s_ECF2;
      static const SG::AuxElement::ConstAccessor<float> s_ECF3;

      // generic accessors used
      static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> s_parent;

      // generic decorations used
      const SG::AuxElement::Decorator<int> m_isB;
      const SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > > m_matchedMuonsLink;
      const SG::AuxElement::Decorator<TLorentzVector> m_correctedJetDecor;
      const SG::AuxElement::Decorator<std::pair<float, float>> m_massWindow;
      const SG::AuxElement::Decorator<std::pair<float, std::string>> m_D2Pivot;
  };
}

#endif
