/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "FlavorTagDiscriminants/BTagMuonAugmenter.h"
#include "FlavorTagDiscriminants/BTagTrackAugmenter.h"

namespace FlavorTagDiscriminants {

  BTagMuonAugmenter::BTagMuonAugmenter( std::string muonAssociationName, float muonMinDR, float muonMinpT ):
    dec_muon_isDefaults("softMuon_isDefaults"),
    dec_muon_pt("softMuon_pt"),
    dec_muon_dR("softMuon_dR"),
    dec_muon_eta("softMuon_eta"),
    dec_muon_phi("softMuon_phi"),
    dec_muon_qOverPratio("softMuon_qOverPratio"),
    dec_muon_mombalsignif("softMuon_momentumBalanceSignificance"),
    dec_muon_scatneighsignif("softMuon_scatteringNeighbourSignificance"),
    dec_muon_pTrel("softMuon_pTrel"),
    dec_muon_ip3d_d0("softMuon_ip3dD0"),
    dec_muon_ip3d_z0("softMuon_ip3dZ0"),
    dec_muon_ip3d_d0_significance("softMuon_ip3dD0Significance"),
    dec_muon_ip3d_z0_significance("softMuon_ip3dZ0Significance"),
    dec_muon_ip3d_sigma_d0("softMuon_ip3dD0Uncertainty"),
    dec_muon_ip3d_sigma_z0("softMuon_ip3dZ0Uncertainty"),
    dec_muon_link("softMuon_link")
  {
    // you probably have to initialize something here
    using namespace FlavorTagDiscriminants;
    btag_track_aug = BTagTrackAugmenter(); //NEED TO CHECK HOW TO PASS SCHEMA
    m_muonAssociationName = muonAssociationName;
    m_muonMinDR = muonMinDR;
    m_muonMinpT = muonMinpT;
  }

  BTagMuonAugmenter::~BTagMuonAugmenter() = default;
  BTagMuonAugmenter::BTagMuonAugmenter(BTagMuonAugmenter&&) = default;

  void BTagMuonAugmenter::augment(const xAOD::Jet &jet ) const {

    BTagSignedIP muon_ip;
    unsigned int muon_index = 99;
    float min_muon_dr = 10;

    //Future decorations
    char  muon_isDefaults = 1;
    float muon_pt = -1;
    float muon_dR = -1;
    float muon_eta = -1;
    float muon_phi = -1;
    float muon_qOverPratio = -1;
    float muon_mombalsignif = -1;
    float muon_scatneighsignif = -1;
    float muon_pTrel = -1;
    float muon_ip3d_d0 = -1;
    float muon_ip3d_z0 = -1;
    float muon_ip3d_d0_significance = -1;
    float muon_ip3d_z0_significance = -1;
    float muon_ip3d_sigma_d0 = -1;
    float muon_ip3d_sigma_z0 = -1;
    ElementLink<xAOD::MuonContainer> muon_link;


    const xAOD::BTagging &btag = *jet.btagging();

    // Find associated combined muon closest to jet axis (like legacy SMT)
    std::vector<ElementLink<xAOD::MuonContainer> > assocMuons;
    assocMuons = btag.auxdata<std::vector<ElementLink<xAOD::MuonContainer> > >(m_muonAssociationName);

    if (assocMuons.size() > 0) {
      for (unsigned int imu = 0; imu < assocMuons.size(); imu++) {
        const xAOD::Muon* thisMu = *( assocMuons.at( imu ) );

        //Check if it's an ID+MS muon
        if ( thisMu->muonType() != xAOD::Muon::Combined) continue;

        //Check minimum muon pT
        if (thisMu->p4().Pt() < m_muonMinpT ) continue;

        //Check if it's inside jet
        float dR = jet.p4().DeltaR(thisMu->p4());
        if (dR > m_muonMinDR) continue;

        //Check that ID and MS track links are good
        const ElementLink< xAOD::TrackParticleContainer >& IDMuTrack_link = thisMu->inDetTrackParticleLink();
        const ElementLink< xAOD::TrackParticleContainer >& MSMuTrack_link = thisMu->muonSpectrometerTrackParticleLink();
        if ( !IDMuTrack_link.isValid() || !MSMuTrack_link.isValid()) continue;
        const xAOD::TrackParticle* IDMuTrack = *IDMuTrack_link;
        const xAOD::TrackParticle* MSMuTrack = *MSMuTrack_link;
        if ( !IDMuTrack || !MSMuTrack) continue;

        //Check quality of muon fit
        float momBalSignif0 = 0;
        thisMu->parameter(momBalSignif0, xAOD::Muon::momentumBalanceSignificance);
        if( momBalSignif0 == 0 ) continue;
        if( (*MSMuTrack_link)->qOverP() == 0 ) continue;

        //Check if this is the closest muon
        if (dR < min_muon_dr) {
          min_muon_dr = dR;
          muon_index = imu;
          muon_link = assocMuons.at( imu );
        }
      }

      if (muon_index < 99) {
        const xAOD::Muon* smtMu = *( assocMuons.at( muon_index ) );
        const xAOD::TrackParticle* IDMuTrack = *(smtMu->inDetTrackParticleLink());
        const xAOD::TrackParticle* MSMuTrack = *(smtMu->muonSpectrometerTrackParticleLink());

        //Get muon info
        muon_isDefaults = 0;
        muon_dR = min_muon_dr;
        muon_pt = smtMu->p4().Pt();
        muon_eta = smtMu->p4().Eta();
        muon_phi = smtMu->p4().Phi();
        muon_qOverPratio = IDMuTrack->qOverP()/MSMuTrack->qOverP();
        smtMu->parameter(muon_mombalsignif, xAOD::Muon::momentumBalanceSignificance);
        smtMu->parameter(muon_scatneighsignif, xAOD::Muon::scatteringNeighbourSignificance);

        TLorentzVector myjet, mymu;
        myjet.SetPtEtaPhiM(jet.p4().Pt(),jet.p4().Eta(),jet.p4().Phi(),0);
        mymu.SetPtEtaPhiM(smtMu->p4().Pt(),smtMu->p4().Eta(),smtMu->p4().Phi(),0);
        muon_pTrel = myjet.Vect().Perp(mymu.Vect()); // VD: everything MUST be in MeV

        //Muon ID track IP information
        muon_ip = btag_track_aug.get_signed_ip(*IDMuTrack, jet);
        muon_ip3d_d0 = muon_ip.ip3d_signed_d0;
        muon_ip3d_z0 = muon_ip.ip3d_signed_z0;
        muon_ip3d_d0_significance = muon_ip.ip3d_signed_d0_significance;
        muon_ip3d_z0_significance = muon_ip.ip3d_signed_z0_significance;
        muon_ip3d_sigma_d0 = btag_track_aug.d0Uncertainty(*IDMuTrack);
        muon_ip3d_sigma_z0 = btag_track_aug.z0SinThetaUncertainty(*IDMuTrack);

      }

    }

    //Decorate btagging object
    dec_muon_isDefaults(btag) = muon_isDefaults;
    dec_muon_pt(btag) = muon_pt;
    dec_muon_dR(btag) = muon_dR;
    dec_muon_eta(btag) = muon_eta;
    dec_muon_phi(btag) = muon_phi;
    dec_muon_qOverPratio(btag) = muon_qOverPratio;
    dec_muon_mombalsignif(btag) = muon_mombalsignif;
    dec_muon_scatneighsignif(btag) = muon_scatneighsignif;
    dec_muon_pTrel(btag) = muon_pTrel;
    dec_muon_ip3d_d0(btag) = muon_ip3d_d0;
    dec_muon_ip3d_z0(btag) = muon_ip3d_z0;
    dec_muon_ip3d_d0_significance(btag) = muon_ip3d_d0_significance;
    dec_muon_ip3d_z0_significance(btag) = muon_ip3d_z0_significance;
    dec_muon_ip3d_sigma_d0(btag) = muon_ip3d_sigma_d0;
    dec_muon_ip3d_sigma_z0(btag) = muon_ip3d_sigma_z0;
    dec_muon_link(btag) = muon_link;

  }

}
