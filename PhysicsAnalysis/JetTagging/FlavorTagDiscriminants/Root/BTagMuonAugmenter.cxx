/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagMuonAugmenter.h"
#include "FlavorTagDiscriminants/BTagTrackIpAccessor.h"
#include "xAODBTagging/BTaggingUtilities.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

namespace FlavorTagDiscriminants {

  BTagMuonAugmenter::BTagMuonAugmenter(std::string muonAssociationName,
                                       float muonMaxDR,
                                       float muonMinpT,
                                       FlipTagConfig flipConfig)
      : m_btag_track_aug("btagIp_"),
        m_muonSelectionTool(nullptr),
        m_acc_assocMuons(muonAssociationName),
        m_acc_jetLink("jetLink"),
        m_acc_muon_energyLoss("EnergyLoss"),
        m_dec_muon_isDefaults("softMuon_isDefaults"),
        m_dec_muon_failLowPtWorkingPoint("softMuon_failLowPtWorkingPoint"),
        m_dec_muon_quality("softMuon_quality"),
        m_dec_nAssocMuons("softMuon_nAssocMuons"),
        m_dec_muon_dR("softMuon_dR"),
        m_dec_muon_qOverPratio("softMuon_qOverPratio"),
        m_dec_muon_pTrel("softMuon_pTrel"),
        m_dec_muon_ip3d_d0("softMuon_ip3dD0"),
        m_dec_muon_ip3d_z0("softMuon_ip3dZ0"),
        m_dec_muon_ip3d_d0_significance("softMuon_ip3dD0Significance"),
        m_dec_muon_ip3d_z0_significance("softMuon_ip3dZ0Significance"),
        m_dec_muon_ip3d_sigma_d0("softMuon_ip3dD0Uncertainty"),
        m_dec_muon_ip3d_sigma_z0("softMuon_ip3dZ0Uncertainty"),
        m_dec_muon_link("softMuon_link"),
        m_float_IO_variables({"pt",
                              "eta",
                              "phi",
                              "momentumBalanceSignificance",
                              "scatteringNeighbourSignificance",
                              "segmentDeltaEta",
                              "segmentDeltaPhi",
                              "ParamEnergyLoss",
                              "ParamEnergyLossSigmaPlus",
                              "ParamEnergyLossSigmaMinus",
                              "MeasEnergyLoss",
                              "MeasEnergyLossSigma",
                              "CaloMuonScore"}),
        m_unsigned_char_IO_variables({"numberOfInnermostPixelLayerHits",
                                      "numberOfInnermostPixelLayerSplitHits",
                                      "numberOfInnermostPixelLayerSharedHits",
                                      "numberOfInnermostPixelLayerOutliers",
                                      "numberOfNextToInnermostPixelLayerHits",
                                      "numberOfNextToInnermostPixelLayerOutliers",
                                      "numberOfPixelHits",
                                      "numberOfPixelSplitHits",
                                      "numberOfPixelSharedHits",
                                      "numberOfPixelSpoiltHits",
                                      "numberOfPixelHoles",
                                      "numberOfSCTHits",
                                      "numberOfSCTSharedHits",
                                      "numberOfSCTHoles",
                                      "expectInnermostPixelLayerHit",
                                      "expectNextToInnermostPixelLayerHit"}) {
    // you probably have to initialize something here
    using namespace FlavorTagDiscriminants;
    m_muonAssociationName = muonAssociationName;
    m_muonMaxDR = muonMaxDR;
    m_muonMinpT = muonMinpT;
    m_flip_config = flipConfig;

    m_muonSelectionTool.reset(new CP::MuonSelectionTool("MuonSelectionTool"));
    StatusCode sc_init = m_muonSelectionTool->initialize();
    if (!sc_init.isSuccess()) {
      throw std::runtime_error("Failed to initialize MuonSelectionTool");
    }
    StatusCode sc_prop = m_muonSelectionTool->setProperty("TurnOffMomCorr", true);
    if (!sc_prop.isSuccess()) {
      throw std::runtime_error("Failed to TurnOffMomCorr for MuonSelectionTool");
    }
    // Fill vectors holding objects that have the const accessors
    // and decorators available as members for each variable
    // that is decorated only.
    for (std::string var : m_float_IO_variables) {
      m_float_IO.emplace_back(CAccDec<float>(var));
    }
    for (std::string var : m_unsigned_char_IO_variables) {
      m_unsigned_char_IO.emplace_back(CAccDec<unsigned char>(var));
    }
  }

  BTagMuonAugmenter::~BTagMuonAugmenter() = default;

  void BTagMuonAugmenter::augment(const xAOD::BTagging& btag) const {
    BTagSignedIP muon_ip;
    int muon_index = -1;
    float dR_closest_muon = 10;

    // set defaults for new variables
    char muon_isDefaults = 1;
    char muon_failLowPtWorkingPoint = 1;
    int muon_quality = -1;
    int nAssocMuons = -1;
    float muon_dR = -1;
    float muon_qOverPratio = -1;
    float muon_pTrel = -1;
    float muon_ip3d_d0 = -1;
    float muon_ip3d_z0 = -1;
    float muon_ip3d_d0_significance = -1;
    float muon_ip3d_z0_significance = -1;
    float muon_ip3d_sigma_d0 = -1;
    float muon_ip3d_sigma_z0 = -1;
    ElementLink<xAOD::MuonContainer> muon_link;

    // set defaults
    for (auto var : m_float_IO) {
      var.m_dec(btag) = -1;
    }
    for (auto var : m_unsigned_char_IO) {
      var.m_dec(btag) = 0;
    }

    // get jet link
    auto jet_link = m_acc_jetLink(btag);
    if (!jet_link.isValid()) {
      throw std::runtime_error("missing jetLink");
    }
    const xAOD::Jet& jet = **jet_link;

    // Find associated combined muon closest to jet axis
    std::vector<ElementLink<xAOD::MuonContainer>> assocMuons =
        m_acc_assocMuons(btag);
    if (assocMuons.size() > 0) {
      nAssocMuons = assocMuons.size();
      for (unsigned int imu = 0; imu < assocMuons.size(); imu++) {
        const xAOD::Muon* thisMu = *(assocMuons.at(imu));
        // Check if it's an ID+MS muon
        if (thisMu->muonType() != xAOD::Muon::Combined)
          continue;

        // Check minimum muon pT
        if (thisMu->pt() < m_muonMinpT)
          continue;

        // Check if it's inside jet
        float dR = jet.p4().DeltaR(thisMu->p4());
        if (dR > m_muonMaxDR)
          continue;

        // Check that ID and MS tracks are good
        const xAOD::TrackParticle* IDMuTrack =
            thisMu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
        const xAOD::TrackParticle* MSMuTrack =
            thisMu->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
        if (!IDMuTrack || !MSMuTrack)
          continue;

        // Check quality of muon fit
        float momBalSignif0 = 0;
        thisMu->parameter(momBalSignif0, xAOD::Muon::momentumBalanceSignificance);
        if (momBalSignif0 == 0)
          continue;
        if (MSMuTrack->qOverP() == 0)
          continue;

        // Check if this is the closest muon
        if (dR < dR_closest_muon) {
          dR_closest_muon = dR;
          muon_index = imu;
          muon_link = assocMuons.at(imu);
        }
      }

      if (muon_index != -1) {
        // Get info for selected muon
        const xAOD::Muon* smtMu = *(assocMuons.at(muon_index));
        const xAOD::TrackParticle* IDMuTrack =
            smtMu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);

        // for MCP the ExtrapolatedMuonSpectrometer is standard for MS track
        const xAOD::TrackParticle* MSMuTrack =
            smtMu->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);

        muon_isDefaults = 0;
        muon_quality = m_muonSelectionTool->getQuality(*smtMu);
        if (m_muonSelectionTool->passedLowPtEfficiencyCuts(*smtMu)) {
          muon_failLowPtWorkingPoint = 0;
        }
        muon_dR = dR_closest_muon;
        muon_qOverPratio = IDMuTrack->qOverP() / MSMuTrack->qOverP();

        // calculate ptrel
        // estimate energy loss in transverse direction
        double muon_energyLoss = m_acc_muon_energyLoss(*smtMu);
        // theta is w.r.t z-axis, therefore sin
        double muon_transverseEnergyLoss = muon_energyLoss * std::sin(smtMu->p4().Theta());
        TLorentzVector muon_loss;
        // E ~ P for muons
        muon_loss.SetPtEtaPhiE(muon_transverseEnergyLoss, smtMu->p4().Eta(), jet.p4().Phi(), muon_energyLoss);
        // subtract muon energy loss once, as it is contained in both
        // the muon and the jet.
        TLorentzVector corrected_jet;
        corrected_jet = jet.p4() - muon_loss + smtMu->p4();
        muon_pTrel = smtMu->p4().Vect().Perp(corrected_jet.Vect());

        // Muon ID track IP information
        muon_ip = m_btag_track_aug.getSignedIp(*IDMuTrack, jet);
        muon_ip3d_d0 = muon_ip.ip3d_signed_d0;
        muon_ip3d_z0 = muon_ip.ip3d_signed_z0;
        muon_ip3d_d0_significance = muon_ip.ip3d_signed_d0_significance;
        muon_ip3d_z0_significance = muon_ip.ip3d_signed_z0_significance;
        muon_ip3d_sigma_d0 = m_btag_track_aug.d0Uncertainty(*IDMuTrack);
        muon_ip3d_sigma_z0 = m_btag_track_aug.z0SinThetaUncertainty(*IDMuTrack);

        // Loop over objects and retrieve the according variable
        // from the smtMu/IDMuTrack object and decorate the btag object
        for (auto var : m_float_IO) {
          var.m_dec(btag) = var.m_acc(*smtMu);
        }
        for (auto var : m_unsigned_char_IO) {
          var.m_dec(btag) = var.m_acc(*IDMuTrack);
        }
      }
    }
    // Decorate btagging object
    m_dec_muon_isDefaults(btag) = muon_isDefaults;
    m_dec_muon_failLowPtWorkingPoint(btag) = muon_failLowPtWorkingPoint;
    m_dec_muon_quality(btag) = muon_quality;
    m_dec_nAssocMuons(btag) = nAssocMuons;
    m_dec_muon_dR(btag) = muon_dR;
    m_dec_muon_qOverPratio(btag) = muon_qOverPratio;
    m_dec_muon_pTrel(btag) = muon_pTrel;
    m_dec_muon_ip3d_d0(btag) = muon_ip3d_d0;
    m_dec_muon_ip3d_z0(btag) = muon_ip3d_z0;
    m_dec_muon_ip3d_d0_significance(btag) = muon_ip3d_d0_significance;
    m_dec_muon_ip3d_z0_significance(btag) = muon_ip3d_z0_significance;
    m_dec_muon_ip3d_sigma_d0(btag) = muon_ip3d_sigma_d0;
    m_dec_muon_ip3d_sigma_z0(btag) = muon_ip3d_sigma_z0;
    m_dec_muon_link(btag) = muon_link;
  }

  std::set<std::string> BTagMuonAugmenter::getDecoratorKeys() const {
    const auto& type_registry = SG::AuxTypeRegistry::instance();
    std::set<std::string> keys;
    for (const auto& auxid : {
             m_dec_muon_isDefaults.auxid(),
             m_dec_muon_dR.auxid(),
             m_dec_muon_qOverPratio.auxid(),
             m_dec_muon_pTrel.auxid(),
             m_dec_muon_ip3d_d0.auxid(),
             m_dec_muon_ip3d_z0.auxid(),
             m_dec_muon_ip3d_d0_significance.auxid(),
             m_dec_muon_ip3d_z0_significance.auxid(),
             m_dec_muon_ip3d_sigma_d0.auxid(),
             m_dec_muon_ip3d_sigma_z0.auxid(),
             m_dec_muon_link.auxid()}) {
      keys.insert(type_registry.getName(auxid));
    }
    return keys;
  }

  std::set<std::string> BTagMuonAugmenter::getAuxInputKeys() const {
    const auto& type_registry = SG::AuxTypeRegistry::instance();
    return {
        m_muonAssociationName,
        type_registry.getName(m_acc_jetLink.auxid())};
  }
  std::set<std::string> BTagMuonAugmenter::getConstituentAuxInputKeys() const {
    return m_btag_track_aug.getTrackIpDataDependencyNames();
  }

}  // namespace FlavorTagDiscriminants
