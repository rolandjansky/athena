// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_MUON_AUGMENTER_H
#define BTAG_MUON_AUGMENTER_H

#include "xAODJet/Jet.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "FlavorTagDiscriminants/EDMSchemaEnums.h"
#include "FlavorTagDiscriminants/BTagTrackAugmenter.h"

namespace FlavorTagDiscriminants {

  class BTagMuonAugmenter
  {
  public:
    // typedef FlavorTagDiscriminants::EDMSchema EDMSchema;
    BTagMuonAugmenter(std::string muonAssociationName, float muonMinDR, float muonMinpT);
    ~BTagMuonAugmenter();
    BTagMuonAugmenter(BTagMuonAugmenter&&);
    void augment(const xAOD::Jet& jet) const;
  private:
    // You'll probably have to add some accessors here
    BTagTrackAugmenter btag_track_aug;
    std::string m_muonAssociationName;
    float m_muonMinDR;
    float m_muonMinpT;

    typedef SG::AuxElement AE;
    AE::Decorator<char>  dec_muon_isDefaults;
    AE::Decorator<float> dec_muon_pt;
    AE::Decorator<float> dec_muon_dR;
    AE::Decorator<float> dec_muon_eta;
    AE::Decorator<float> dec_muon_phi;
    AE::Decorator<float> dec_muon_qOverPratio;
    AE::Decorator<float> dec_muon_mombalsignif;
    AE::Decorator<float> dec_muon_scatneighsignif;
    AE::Decorator<float> dec_muon_pTrel;
    AE::Decorator<float> dec_muon_ip3d_d0;
    AE::Decorator<float> dec_muon_ip3d_z0;
    AE::Decorator<float> dec_muon_ip3d_d0_significance;
    AE::Decorator<float> dec_muon_ip3d_z0_significance;
    AE::Decorator<float> dec_muon_ip3d_sigma_d0;
    AE::Decorator<float> dec_muon_ip3d_sigma_z0;
    AE::Decorator< ElementLink<xAOD::MuonContainer> > dec_muon_link;
  };

}
#endif
