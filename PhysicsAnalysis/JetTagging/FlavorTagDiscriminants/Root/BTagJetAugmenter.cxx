/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagJetAugmenter.h"

#include <cmath>
#include <cstddef>

#include "xAODJet/Jet.h"

#include "TVector3.h"

namespace {
  // grab names based on configuration
  std::string negString(FlavorTagDiscriminants::FlipTagConfig f) {
    using namespace FlavorTagDiscriminants;
    switch(f) {
    case FlipTagConfig::STANDARD: return "";
    case FlipTagConfig::FLIP_SIGN: // intentional fall-through
    case FlipTagConfig::NEGATIVE_IP_ONLY: return "Neg";
    default: throw std::logic_error("undefined flip config");
    }
  }
  std::string flipString(FlavorTagDiscriminants::FlipTagConfig f) {
    using namespace FlavorTagDiscriminants;
    switch(f) {
    case FlipTagConfig::STANDARD: return "";
    case FlipTagConfig::FLIP_SIGN: // intentional fall-through
    case FlipTagConfig::NEGATIVE_IP_ONLY: return "Flip";
    default: throw std::logic_error("undefined flip config");
    }
  }

  // the taggers
  std::string ip2(FlavorTagDiscriminants::FlipTagConfig f) {
    return "IP2D" + negString(f);
  }
  std::string ip3(FlavorTagDiscriminants::FlipTagConfig f) {
    return "IP3D" + negString(f);
  }
  std::string rnn(FlavorTagDiscriminants::FlipTagConfig f) {
    using namespace FlavorTagDiscriminants;
    switch(f) {
    case FlipTagConfig::STANDARD: return "rnnip";
    case FlipTagConfig::FLIP_SIGN: // intentional fall-through
    case FlipTagConfig::NEGATIVE_IP_ONLY: return "rnnipflip";
    default: throw std::logic_error("undefined flip config");
    }
  }
  std::string jf(FlavorTagDiscriminants::FlipTagConfig f) {
    return "JetFitter" + flipString(f);
  }
  std::string sv(FlavorTagDiscriminants::FlipTagConfig f) {
    return "SV1" + flipString(f);
  }
  std::string jfSv(FlavorTagDiscriminants::FlipTagConfig f) {
    return "secondaryVtx" + flipString(f);
  }
  std::string jfSvNew(FlavorTagDiscriminants::FlipTagConfig f) {
    return "JetFitterSecondaryVertex" + flipString(f);
  }

}

BTagJetAugmenter::BTagJetAugmenter(FlavorTagDiscriminants::EDMSchema s,
                                   FlavorTagDiscriminants::FlipTagConfig f):
  m_use_floats(false),
  m_write_unflippable(true),
  pt_uncalib("pt_uncalib"),
  eta_uncalib("eta_uncalib"),
  abs_eta_uncalib("abs_eta_uncalib"),
  ip2d_weightBOfTracks(ip2(f) + "_weightBofTracks"),
  ip2d_nTrks(ip2(f) + "_nTrks"),
  ip2d_pu(ip2(f) + "_pu"),
  ip2d_pc(ip2(f) + "_pc"),
  ip2d_pb(ip2(f) + "_pb"),
  ip2d_isDefaults(ip2(f) + "_isDefaults"),
  ip2d_cu(ip2(f) + "_cu"),
  ip2d_bu(ip2(f) + "_bu"),
  ip2d_bc(ip2(f) + "_bc"),
  ip3d_weightBOfTracks(ip3(f) + "_weightBofTracks"),
  ip3d_nTrks(ip3(f) + "_nTrks"),
  ip3d_pu(ip3(f) + "_pu"),
  ip3d_pc(ip3(f) + "_pc"),
  ip3d_pb(ip3(f) + "_pb"),
  ip3d_isDefaults(ip3(f) + "_isDefaults"),
  ip3d_cu(ip3(f) + "_cu"),
  ip3d_bu(ip3(f) + "_bu"),
  ip3d_bc(ip3(f) + "_bc"),
  jf_deltaEta(jf(f) + "_deltaeta"),
  jf_deltaPhi(jf(f) + "_deltaphi"),
  jf_fittedPosition(jf(f) + "_fittedPosition"),
  jf_vertices(jf(f) + "_JFvertices"),
  jf_nVtx(jf(f) + "_nVTX"),
  jf_nSingleTracks(jf(f) + "_nSingleTracks"),
  jf_isDefaults(jf(f) + "_isDefaults"),
  jf_deltaR(jf(f) + "_deltaR"),
  sv1_vertices(sv(f) + "_vertices"),
  sv1_nVtx(sv(f) + "_nVtx"),
  sv1_isDefaults(sv(f) + "_isDefaults"),
  jet_track_links("BTagTrackToJetAssociator"),
  secondaryVtx_isDefaults(jfSv(f) + "_isDefaults"),
  secondaryVtx_nTrks(jfSv(f) + "_nTrks"),
  secondaryVtx_m(jfSv(f) + "_m"),
  secondaryVtx_E(jfSv(f) + "_E"),
  secondaryVtx_EFrac(jfSv(f) + "_EFrac"),
  secondaryVtx_L3d(jfSv(f) + "_L3d"),
  secondaryVtx_Lxy(jfSv(f) + "_Lxy"),
  secondaryVtx_min_trk_flightDirRelEta(jfSv(f) + "_min_trk_flightDirRelEta"),
  secondaryVtx_max_trk_flightDirRelEta(jfSv(f) + "_max_trk_flightDirRelEta"),
  secondaryVtx_avg_trk_flightDirRelEta(jfSv(f) + "_avg_trk_flightDirRelEta"),
  min_trk_flightDirRelEta("min_trk_flightDirRelEta" + flipString(f)),
  max_trk_flightDirRelEta("max_trk_flightDirRelEta" + flipString(f)),
  avg_trk_flightDirRelEta("avg_trk_flightDirRelEta" + flipString(f)),
  smt_mu_pt("SMT_mu_pt"),
  smt_isDefaults("SMT_isDefaults"),
  rnnip_pbIsValid(rnn(f) + "_pbIsValid"),
  rnnip_isDefaults(rnn(f) + "_isDefaults"),
  new_secondaryVtx_m(jfSvNew(f) + "_mass"),
  new_secondaryVtx_E(jfSvNew(f) + "_energy"),
  new_secondaryVtx_EFrac(jfSvNew(f) + "_energyFraction"),
  new_secondaryVtx_min_trk_flightDirRelEta(jfSvNew(f) + "_minimumTrackRelativeEta"),
  new_secondaryVtx_max_trk_flightDirRelEta(jfSvNew(f) + "_maximumTrackRelativeEta"),
  new_secondaryVtx_avg_trk_flightDirRelEta(jfSvNew(f) + "_averageTrackRelativeEta"),
  new_min_trk_flightDirRelEta("minimumTrackRelativeEta" + flipString(f)),
  new_max_trk_flightDirRelEta("maximumTrackRelativeEta" + flipString(f)),
  new_avg_trk_flightDirRelEta("averageTrackRelativeEta" + flipString(f))
{
  using namespace FlavorTagDiscriminants;
  typedef SG::AuxElement::Decorator<float> ADF;
  typedef SG::AuxElement::Decorator<double> ADD;
  typedef SG::AuxElement::Decorator<char> ADC;
  typedef SG::AuxElement::Decorator<int> ADI;
  if (s != EDMSchema::WINTER_2018) {
    pt_uncalib = ADD("pt_btagJes");
    eta_uncalib = ADD("eta_btagJes");
    abs_eta_uncalib = ADD("absEta_btagJes");
    secondaryVtx_isDefaults = ADC(jfSvNew(f) + "_isDefaults");
    secondaryVtx_nTrks = ADI(jfSvNew(f) + "_nTracks");
    secondaryVtx_L3d = ADF(jfSvNew(f) + "_displacement3d");
    secondaryVtx_Lxy = ADF(jfSvNew(f) + "_displacement2d");
    m_use_floats = true;
  }

}

BTagJetAugmenter::~BTagJetAugmenter() = default;
BTagJetAugmenter::BTagJetAugmenter(BTagJetAugmenter&&) = default;

void BTagJetAugmenter::augment(const xAOD::Jet &jet, const xAOD::Jet &uncalibrated_jet) {

  const xAOD::BTagging* btag_ptr = jet.btagging();
  if (!btag_ptr) throw std::runtime_error("No b-tagging object found!");
  const xAOD::BTagging& btag = *btag_ptr;

  if (m_write_unflippable) {
    pt_uncalib(btag) = uncalibrated_jet.pt();
    eta_uncalib(btag) = uncalibrated_jet.eta();
    abs_eta_uncalib(btag) = std::abs(uncalibrated_jet.eta());
  }
  // pass off to calibrated jet function
  augment(jet);
}

void BTagJetAugmenter::augmentJfDr(const xAOD::BTagging& btag) {
  if (jfIsDefaults(btag)) {
    jf_deltaR(btag) = NAN;
  } else {
    jf_deltaR(btag) = std::hypot(jf_deltaEta(btag), jf_deltaPhi(btag));
  }
}
void BTagJetAugmenter::augmentIpRatios(const xAOD::BTagging& btag) {

  ip2d_cu(btag) = std::log(ip2d_pc(btag) / ip2d_pu(btag));
  ip2d_bu(btag) = std::log(ip2d_pb(btag) / ip2d_pu(btag));
  ip2d_bc(btag) = std::log(ip2d_pb(btag) / ip2d_pc(btag));

  ip3d_cu(btag) = std::log(ip3d_pc(btag) / ip3d_pu(btag));
  ip3d_bu(btag) = std::log(ip3d_pb(btag) / ip3d_pu(btag));
  ip3d_bc(btag) = std::log(ip3d_pb(btag) / ip3d_pc(btag));

}

void BTagJetAugmenter::augment(const xAOD::Jet &jet) {
  const xAOD::BTagging* btag_ptr = jet.btagging();
  if (!btag_ptr) throw std::runtime_error("No b-tagging object found!");
  const xAOD::BTagging& btag = *btag_ptr;

  if (ip2d_weightBOfTracks(btag).size() > 0) {
    ip2d_isDefaults(btag) = 0;
  } else {
    ip2d_isDefaults(btag) = 1;
  }
  ip2d_nTrks(btag) = ip2d_weightBOfTracks(btag).size();

  if (ip3d_weightBOfTracks(btag).size() > 0) {
    ip3d_isDefaults(btag) = 0;
  } else {
    ip3d_isDefaults(btag) = 1;
  }

  ip3d_nTrks(btag) = ip3d_weightBOfTracks(btag).size();
  augmentIpRatios(btag);

  jf_isDefaults(btag) = jfIsDefaults(btag);
  augmentJfDr(btag);

  if (sv1_vertices(btag).size() > 0) {
    sv1_isDefaults(btag) = 0;
  } else {
    sv1_isDefaults(btag) = 1;
  }

  TVector3 flightDir(NAN, NAN, NAN);
  float jf_phi = NAN;
  float jf_theta = NAN;

  int secondary_jf_vtx_index = -1;
  int secondaryVtx_track_number = -1;
  double secondaryVtx_track_flightDirRelEta_total = NAN;
  float min_jf_vtx_L3d = NAN;

  if (jf_fittedPosition(btag).size() > 4) {
    jf_phi = jf_fittedPosition(btag).at(3);
    jf_theta = jf_fittedPosition(btag).at(4);
    flightDir.SetMagThetaPhi(1, jf_theta, jf_phi);

    for (std::size_t jf_vtx_index = 0; jf_vtx_index < jf_vertices(btag).size() && jf_vtx_index < jf_fittedPosition(btag).size() - 5; jf_vtx_index++) {
      float jf_vtx_L3d = jf_fittedPosition(btag).at(jf_vtx_index + 5);
      if (jf_vtx_L3d > 0 && (jf_vtx_L3d < min_jf_vtx_L3d || std::isnan(min_jf_vtx_L3d))) {
        secondary_jf_vtx_index = jf_vtx_index;
        min_jf_vtx_L3d = jf_vtx_L3d;
      }
    }

    if (secondary_jf_vtx_index >= 0) {
      secondaryVtx_track_number = 0;
      secondaryVtx_track_flightDirRelEta_total = 0;
      secondaryVtx_isDefaults(btag) = 0;
    } else {
      secondaryVtx_isDefaults(btag) = 1;
    }
  } else {
    secondaryVtx_isDefaults(btag) = 1;
  }
  secondaryVtx_L3d(btag) = min_jf_vtx_L3d;
  secondaryVtx_Lxy(btag) = min_jf_vtx_L3d * sinf(jf_theta);

  const float track_mass = 139.57; // Why?

  unsigned track_number = 0;
  double track_E_total = 0;
  double min_track_flightDirRelEta = NAN;
  double max_track_flightDirRelEta = NAN;
  double track_flightDirRelEta_total = 0;

  TLorentzVector secondaryVtx_4momentum_total;
  double secondaryVtx_min_track_flightDirRelEta = NAN;
  double secondaryVtx_max_track_flightDirRelEta = NAN;

  for (const auto &jet_track_link : jet_track_links(btag)) {
    const xAOD::TrackParticle &track_particle = **jet_track_link;

    uint8_t n_pixel_hits;
    uint8_t n_sct_hits;
    track_particle.summaryValue(n_pixel_hits, xAOD::numberOfPixelHits);
    track_particle.summaryValue(n_sct_hits, xAOD::numberOfSCTHits);
    if (n_pixel_hits + n_sct_hits < 2) continue;

    track_number++;
    track_E_total += track_particle.e();

    TVector3 track_flightDirRelVect = track_particle.p4().Vect();
    track_flightDirRelVect.SetTheta(track_flightDirRelVect.Angle(flightDir));

    double track_flightDirRelEta = NAN;
    if (track_flightDirRelVect.Perp() != 0) {
      track_flightDirRelEta = track_flightDirRelVect.PseudoRapidity();
    }

    track_flightDirRelEta_total += track_flightDirRelEta;
    if (track_flightDirRelEta < min_track_flightDirRelEta || std::isnan(min_track_flightDirRelEta)) {
      min_track_flightDirRelEta = track_flightDirRelEta;
    }
    if (track_flightDirRelEta > max_track_flightDirRelEta || std::isnan(max_track_flightDirRelEta)) {
      max_track_flightDirRelEta = track_flightDirRelEta;
    }
    if (secondary_jf_vtx_index >= 0) {
      for (const ElementLink<xAOD::TrackParticleContainer> vertex_track_particle : (**jf_vertices(btag).at(secondary_jf_vtx_index)).track_links()) {
        if (*vertex_track_particle == &track_particle) {
          secondaryVtx_track_number++;
          TLorentzVector track_fourVector;
          track_fourVector.SetVectM(track_particle.p4().Vect(), track_mass);
          secondaryVtx_4momentum_total += track_fourVector;
          secondaryVtx_track_flightDirRelEta_total += track_flightDirRelEta;
          if (track_flightDirRelEta < secondaryVtx_min_track_flightDirRelEta || std::isnan(secondaryVtx_min_track_flightDirRelEta)) {
            secondaryVtx_min_track_flightDirRelEta = track_flightDirRelEta;
          }
          if (track_flightDirRelEta > secondaryVtx_max_track_flightDirRelEta || std::isnan(secondaryVtx_max_track_flightDirRelEta)) {
            secondaryVtx_max_track_flightDirRelEta = track_flightDirRelEta;
          }
        }
      }
    }
  }

  secondaryVtx_nTrks(btag) = secondaryVtx_track_number;

  // Here begins a few blocks where the decoration depends on the
  // schema. I would like to move a number of values that were
  // previously stored as double to float.
  {
    double m = NAN;
    double E = NAN;
    double EFrac = NAN;
    if (secondaryVtx_track_number >= 0) {
      m = secondaryVtx_4momentum_total.M();
      E = secondaryVtx_4momentum_total.E();
      EFrac = secondaryVtx_4momentum_total.E() / track_E_total;
    }
    if (m_use_floats) {
      new_secondaryVtx_m(btag) = m;
      new_secondaryVtx_E(btag) = E;
      new_secondaryVtx_EFrac(btag) = EFrac;
    } else {
      secondaryVtx_m(btag) = m;
      secondaryVtx_E(btag) = E;
      secondaryVtx_EFrac(btag) = EFrac;
    }
  }
  {
    double min = secondaryVtx_min_track_flightDirRelEta;
    double max = secondaryVtx_max_track_flightDirRelEta;
    double avg = secondaryVtx_track_flightDirRelEta_total / secondaryVtx_track_number;
    if (m_use_floats) {
      new_secondaryVtx_min_trk_flightDirRelEta(btag) = min;
      new_secondaryVtx_max_trk_flightDirRelEta(btag) = max;
      new_secondaryVtx_avg_trk_flightDirRelEta(btag) = avg;
    } else {
      secondaryVtx_min_trk_flightDirRelEta(btag) = min;
      secondaryVtx_max_trk_flightDirRelEta(btag) = max;
      secondaryVtx_avg_trk_flightDirRelEta(btag) = avg;
    }
  }
  {
    double min = min_track_flightDirRelEta;
    double max = max_track_flightDirRelEta;
    double avg = track_flightDirRelEta_total / track_number;
    if (m_write_unflippable) {
      if (m_use_floats) {
        new_min_trk_flightDirRelEta(btag) = min;
        new_max_trk_flightDirRelEta(btag) = max;
        new_avg_trk_flightDirRelEta(btag) = avg;
      } else {
        min_trk_flightDirRelEta(btag) = min;
        max_trk_flightDirRelEta(btag) = max;
        avg_trk_flightDirRelEta(btag) = avg;
      }
    }
  }
  if (m_write_unflippable) {
    if (smt_mu_pt(btag) > 0) {
      smt_isDefaults(btag) = 0;
    }  else {
      smt_isDefaults(btag) = 1;
    }
  }

  // pbIsValid is only defined in the "old" schema, which does not use
  // floats. This will short circuit if we do use floats.
  if (!m_use_floats && !rnnip_pbIsValid(btag)) {
    rnnip_isDefaults(btag) = 1;
  }  else {
    rnnip_isDefaults(btag) = 0;
  }
}


bool BTagJetAugmenter::jfIsDefaults(const xAOD::BTagging& btag) {
  return !(jf_vertices(btag).size() > 0 && (jf_nVtx(btag) > 0 || jf_nSingleTracks(btag) > 0));
}
