// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_MUON_AUGMENTER_H
#define BTAG_MUON_AUGMENTER_H

#include "FlavorTagDiscriminants/BTagTrackIpAccessor.h"
#include "FlavorTagDiscriminants/FlipTagEnums.h"
#include "xAODBTagging/BTaggingFwd.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"

#include "MuonSelectorTools/MuonSelectionTool.h"

namespace FlavorTagDiscriminants {

  // Accessor and Decorator pair template for IO purposes where you
  // just want to decorate "someVariable" to the btagging object as
  // "softMuon_someVariable"
  template <typename T>
  class CAccDec {
   public:
    CAccDec(const std::string& branchname)
        : m_acc(branchname),
          m_dec("softMuon_" + branchname){};

    SG::AuxElement::ConstAccessor<T> m_acc;
    SG::AuxElement::Decorator<T> m_dec;
  };

  namespace defaults {
    const float MUON_MAX_DR = 0.4;
    const float MUON_MIN_PT = 4e3;
  }  // namespace defaults
  typedef SG::AuxElement AE;

  class BTagMuonAugmenter {
   public:
    BTagMuonAugmenter(std::string muonAssociationName,
                      float muonMaxDR = defaults::MUON_MAX_DR,
                      float muonMinpT = defaults::MUON_MIN_PT,
                      FlipTagConfig = FlipTagConfig::STANDARD);
    ~BTagMuonAugmenter();
    BTagMuonAugmenter(BTagMuonAugmenter&&);
    void augment(const xAOD::BTagging& btag) const;
    std::set<std::string> getDecoratorKeys() const;
    std::set<std::string> getAuxInputKeys() const;
    std::set<std::string> getConstituentAuxInputKeys() const;

   private:
    // You'll probably have to add some accessors here
    BTagTrackIpAccessor m_btag_track_aug;
    std::string m_muonAssociationName;
    float m_muonMaxDR;
    float m_muonMinpT;
    FlipTagConfig m_flip_config;

    std::unique_ptr<CP::MuonSelectionTool> m_muonSelectionTool;

    AE::ConstAccessor<std::vector<ElementLink<xAOD::MuonContainer>>> m_acc_assocMuons;
    AE::ConstAccessor<ElementLink<xAOD::JetContainer>> m_acc_jetLink;
    AE::ConstAccessor<float> m_acc_muon_energyLoss;

    AE::Decorator<char> m_dec_muon_isDefaults;
    AE::Decorator<char> m_dec_muon_failLowPtWorkingPoint;
    AE::Decorator<int> m_dec_muon_quality;
    AE::Decorator<int> m_dec_nAssocMuons;
    AE::Decorator<float> m_dec_muon_dR;
    AE::Decorator<float> m_dec_muon_qOverPratio;
    AE::Decorator<float> m_dec_muon_pTrel;
    AE::Decorator<float> m_dec_muon_ip3d_d0;
    AE::Decorator<float> m_dec_muon_ip3d_z0;
    AE::Decorator<float> m_dec_muon_ip3d_d0_significance;
    AE::Decorator<float> m_dec_muon_ip3d_z0_significance;
    AE::Decorator<float> m_dec_muon_ip3d_sigma_d0;
    AE::Decorator<float> m_dec_muon_ip3d_sigma_z0;
    AE::Decorator<ElementLink<xAOD::MuonContainer>> m_dec_muon_link;

    // list of variables that are just accessed and decorated with
    // name softMuon_someVariable
    std::vector<std::string> m_float_IO_variables;
    // vector holding the accessors and decorators
    std::vector<CAccDec<float>> m_float_IO;

    // same for unsigned char
    std::vector<std::string> m_unsigned_char_IO_variables;
    std::vector<CAccDec<unsigned char>> m_unsigned_char_IO;
  };

}  // namespace FlavorTagDiscriminants
#endif
