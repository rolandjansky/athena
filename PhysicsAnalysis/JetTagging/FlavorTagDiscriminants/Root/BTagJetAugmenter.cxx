/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagJetAugmenter.h"

#include <cmath>
#include <cstddef>
#include <vector>

#include "xAODJet/Jet.h"
#include "xAODBTagging/BTaggingUtilities.h"

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
  std::string jfSvNew(FlavorTagDiscriminants::FlipTagConfig f) {
    return "JetFitterSecondaryVertex" + flipString(f);
  }

}

BTagJetAugmenter::BTagJetAugmenter(std::string associator, FlavorTagDiscriminants::FlipTagConfig f):
  m_jetLink("jetLink"),
  m_pt_uncalib("pt_btagJes"),
  m_eta_uncalib("eta_btagJes"),
  m_abs_eta_uncalib("absEta_btagJes"),
  m_ip2d_weightBOfTracks(ip2(f) + "_weightBofTracks"),
  m_ip2d_nTrks(ip2(f) + "_nTrks"),
  m_ip2d_pu(ip2(f) + "_pu"),
  m_ip2d_pc(ip2(f) + "_pc"),
  m_ip2d_pb(ip2(f) + "_pb"),
  m_ip2d_isDefaults(ip2(f) + "_isDefaults"),
  m_ip2d_cu(ip2(f) + "_cu"),
  m_ip2d_bu(ip2(f) + "_bu"),
  m_ip2d_bc(ip2(f) + "_bc"),
  m_ip3d_weightBOfTracks(ip3(f) + "_weightBofTracks"),
  m_ip3d_nTrks(ip3(f) + "_nTrks"),
  m_ip3d_pu(ip3(f) + "_pu"),
  m_ip3d_pc(ip3(f) + "_pc"),
  m_ip3d_pb(ip3(f) + "_pb"),
  m_ip3d_isDefaults(ip3(f) + "_isDefaults"),
  m_ip3d_cu(ip3(f) + "_cu"),
  m_ip3d_bu(ip3(f) + "_bu"),
  m_ip3d_bc(ip3(f) + "_bc"),
  m_jf_deltaEta(jf(f) + "_deltaeta"),
  m_jf_deltaPhi(jf(f) + "_deltaphi"),
  m_jf_fittedPosition(jf(f) + "_fittedPosition"),
  m_jf_vertices(jf(f) + "_JFvertices"),
  m_jf_nVtx(jf(f) + "_nVTX"),
  m_jf_nSingleTracks(jf(f) + "_nSingleTracks"),
  m_jf_isDefaults(jf(f) + "_isDefaults"),
  m_jf_deltaR(jf(f) + "_deltaR"),
  m_sv1_vertices(sv(f) + "_vertices"),
  m_sv1_nVtx(sv(f) + "_nVtx"),
  m_sv1_isDefaults(sv(f) + "_isDefaults"),
  m_jet_track_links(associator),
  m_secondaryVtx_isDefaults(jfSvNew(f) + "_isDefaults"),
  m_secondaryVtx_nTrks(jfSvNew(f) + "_nTracks"),
  m_secondaryVtx_m(jfSvNew(f) + "_mass"),
  m_secondaryVtx_E(jfSvNew(f) + "_energy"),
  m_secondaryVtx_EFrac(jfSvNew(f) + "_energyFraction"),
  m_secondaryVtx_L3d(jfSvNew(f) + "_displacement3d"),
  m_secondaryVtx_Lxy(jfSvNew(f) + "_displacement2d"),
  m_secondaryVtx_min_trk_flightDirRelEta(jfSvNew(f) + "_minimumTrackRelativeEta"),
  m_secondaryVtx_max_trk_flightDirRelEta(jfSvNew(f) + "_maximumTrackRelativeEta"),
  m_secondaryVtx_avg_trk_flightDirRelEta(jfSvNew(f) + "_averageTrackRelativeEta"),
  m_secondaryVtx_DmesonMass(jfSvNew(f) + "_DmesonMass"),
  m_secondaryVtx_isDmesonRecon(jfSvNew(f) + "_isDmesonRecon"),
  m_min_trk_flightDirRelEta(jfSvNew(f) + "_minimumAllJetTrackRelativeEta"),
  m_max_trk_flightDirRelEta(jfSvNew(f) + "_maximumAllJetTrackRelativeEta"),
  m_avg_trk_flightDirRelEta(jfSvNew(f) + "_averageAllJetTrackRelativeEta"),
  m_rnnip_pbIsValid(rnn(f) + "_pbIsValid"),
  m_rnnip_isDefaults(rnn(f) + "_isDefaults")
{
}

BTagJetAugmenter::~BTagJetAugmenter() = default;
BTagJetAugmenter::BTagJetAugmenter(BTagJetAugmenter&&) = default;

std::set<std::string> BTagJetAugmenter::getDecoratorKeys() const {
  const auto& type_registry = SG::AuxTypeRegistry::instance();
  std::set<std::string> keys;
  for (const auto& auxid: {
        m_pt_uncalib.auxid(),
        m_eta_uncalib.auxid(),
        m_abs_eta_uncalib.auxid(),
        m_ip2d_nTrks.auxid(),
        m_ip2d_isDefaults.auxid(),
        m_ip2d_cu.auxid(),
        m_ip2d_bu.auxid(),
        m_ip2d_bc.auxid(),
        m_ip3d_nTrks.auxid(),
        m_ip3d_isDefaults.auxid(),
        m_ip3d_cu.auxid(),
        m_ip3d_bu.auxid(),
        m_ip3d_bc.auxid(),
        m_jf_isDefaults.auxid(),
        m_jf_deltaR.auxid(),
        m_sv1_isDefaults.auxid(),
        m_secondaryVtx_isDefaults.auxid(),
        m_secondaryVtx_nTrks.auxid(),
        m_secondaryVtx_m.auxid(),
        m_secondaryVtx_E.auxid(),
        m_secondaryVtx_EFrac.auxid(),
        m_secondaryVtx_L3d.auxid(),
        m_secondaryVtx_Lxy.auxid(),
        m_secondaryVtx_min_trk_flightDirRelEta.auxid(),
        m_secondaryVtx_max_trk_flightDirRelEta.auxid(),
        m_secondaryVtx_avg_trk_flightDirRelEta.auxid(),
        m_secondaryVtx_DmesonMass.auxid(),
        m_secondaryVtx_isDmesonRecon.auxid(),
        m_min_trk_flightDirRelEta.auxid(),
        m_max_trk_flightDirRelEta.auxid(),
        m_avg_trk_flightDirRelEta.auxid(),
        m_rnnip_isDefaults.auxid()}) {
    keys.insert(type_registry.getName(auxid));
  }
  return keys;
}

std::set<std::string> BTagJetAugmenter::getAuxInputKeys() const {
  const auto& type_registry = SG::AuxTypeRegistry::instance();
  std::set<std::string> keys;
  for (const auto& auxid: {
      m_jetLink.auxid()}) {
    keys.insert(type_registry.getName(auxid));
  }
  return keys;
}

void BTagJetAugmenter::augment(const xAOD::BTagging &jet,
                               const xAOD::BTagging &uncalibrated_jet) {

  augmentBtagJes(jet, uncalibrated_jet);

  // pass off to calibrated jet function
  augment(jet);
}

void BTagJetAugmenter::augmentJfDr(const xAOD::BTagging& btag) {
  if (jfIsDefaults(btag)) {
    m_jf_deltaR(btag) = NAN;
  } else {
    m_jf_deltaR(btag) = std::hypot(m_jf_deltaEta(btag), m_jf_deltaPhi(btag));
  }
}


float BTagJetAugmenter::safelog_prob(float p_up, float p_down){

  if( std::isnan(p_up) ){
    return -1000.0;
  }

  if(std::isnan(p_down) ){
    return -1000.0;
  }

  if(p_down < 0.0000000000000000000001 && p_up > p_down){
    return 1000.0;
  }

  if(p_up < 0.0000000000000000000001){
    return -1000.0;
  }

  return std::log(p_up /p_down);
}

void BTagJetAugmenter::augmentIpRatios(const xAOD::BTagging& btag) {

  m_ip2d_cu(btag) = safelog_prob(m_ip2d_pc(btag) , m_ip2d_pu(btag));
  m_ip2d_bu(btag) = safelog_prob(m_ip2d_pb(btag) , m_ip2d_pu(btag));
  m_ip2d_bc(btag) = safelog_prob(m_ip2d_pb(btag) , m_ip2d_pc(btag));

  m_ip3d_cu(btag) = safelog_prob(m_ip3d_pc(btag) , m_ip3d_pu(btag));
  m_ip3d_bu(btag) = safelog_prob(m_ip3d_pb(btag) , m_ip3d_pu(btag));
  m_ip3d_bc(btag) = safelog_prob(m_ip3d_pb(btag) , m_ip3d_pc(btag));

}
void BTagJetAugmenter::augmentBtagJes(const xAOD::BTagging &btag,
                                      const xAOD::BTagging &uncalib_tag) {

  auto uncalib_link = m_jetLink(uncalib_tag);
  if (!uncalib_link.isValid()) {
    throw std::runtime_error("missing jetLink");
  }
  const xAOD::Jet& uncalib = **uncalib_link;

  m_pt_uncalib(btag) = uncalib.pt();
  m_eta_uncalib(btag) = uncalib.eta();
  m_abs_eta_uncalib(btag) = std::abs(uncalib.eta());

}

void BTagJetAugmenter::augment(const xAOD::BTagging &btag) {

  if (m_ip2d_weightBOfTracks(btag).size() > 0) {
    m_ip2d_isDefaults(btag) = 0;
  } else {
    m_ip2d_isDefaults(btag) = 1;
  }
  m_ip2d_nTrks(btag) = m_ip2d_weightBOfTracks(btag).size();

  if (m_ip3d_weightBOfTracks(btag).size() > 0) {
    m_ip3d_isDefaults(btag) = 0;
  } else {
    m_ip3d_isDefaults(btag) = 1;
  }

  m_ip3d_nTrks(btag) = m_ip3d_weightBOfTracks(btag).size();
  augmentIpRatios(btag);

  m_jf_isDefaults(btag) = jfIsDefaults(btag);
  augmentJfDr(btag);

  if (m_sv1_vertices(btag).size() > 0) {
    m_sv1_isDefaults(btag) = 0;
  } else {
    m_sv1_isDefaults(btag) = 1;
  }

  TVector3 flightDir(NAN, NAN, NAN);
  float jf_phi = NAN;
  float jf_theta = NAN;

  int secondary_jf_vtx_index = -1;
  int secondaryVtx_track_number = -1;
  float secondaryVtx_charge = 0; // SV charge!!!
  double secondaryVtx_track_flightDirRelEta_total = NAN;
  float min_jf_vtx_L3d = NAN;

  if (m_jf_fittedPosition(btag).size() > 4) {
    jf_phi = m_jf_fittedPosition(btag).at(3);
    jf_theta = m_jf_fittedPosition(btag).at(4);
    flightDir.SetMagThetaPhi(1, jf_theta, jf_phi);

    for (std::size_t jf_vtx_index = 0; jf_vtx_index < m_jf_vertices(btag).size() && jf_vtx_index < m_jf_fittedPosition(btag).size() - 5; jf_vtx_index++) {
      float jf_vtx_L3d = m_jf_fittedPosition(btag).at(jf_vtx_index + 5);
      if (jf_vtx_L3d > 0){
        if(std::isnan(min_jf_vtx_L3d) || (jf_vtx_L3d < min_jf_vtx_L3d)){ 
          secondary_jf_vtx_index = jf_vtx_index;
          min_jf_vtx_L3d = jf_vtx_L3d;
        }
      }
    }

    if (secondary_jf_vtx_index >= 0) {
      secondaryVtx_track_number = 0;
      secondaryVtx_track_flightDirRelEta_total = 0;
      m_secondaryVtx_isDefaults(btag) = 0;
    } else {
      m_secondaryVtx_isDefaults(btag) = 1;
    }
  } else {
    m_secondaryVtx_isDefaults(btag) = 1;
  }
  m_secondaryVtx_L3d(btag) = min_jf_vtx_L3d;
  m_secondaryVtx_Lxy(btag) = min_jf_vtx_L3d * sinf(jf_theta);

  const float track_mass = 139.57; // assume pion mass for all tracks
  const float track_kaon = 493.677; // kaon mass

  unsigned track_number = 0;
  double track_E_total = 0;
  double min_track_flightDirRelEta = NAN;
  double max_track_flightDirRelEta = NAN;
  double track_flightDirRelEta_total = 0;

  TLorentzVector secondaryVtx_4momentum_total;
  TLorentzVector secondaryVtx_4momentum_Dmeson;

  // try to record 
  std::vector<TLorentzVector> secondaryVtx_4momentum_vector;
  std::vector<float> secondaryVtx_charge_vector;

  double secondaryVtx_min_track_flightDirRelEta = NAN;
  double secondaryVtx_max_track_flightDirRelEta = NAN;

  // Loop over tracks in the jet
  for (const auto &jet_track_link : m_jet_track_links(btag)) {
    const xAOD::TrackParticle &track_particle = **jet_track_link;

    uint8_t n_pixel_hits;
    uint8_t n_sct_hits;
    bool rc = true;
    rc &= track_particle.summaryValue(n_pixel_hits, xAOD::numberOfPixelHits);
    rc &= track_particle.summaryValue(n_sct_hits, xAOD::numberOfSCTHits);
    if (!rc) throw std::runtime_error(
      "track summary values are missing, can't compute b-tagging variables");
    if (n_pixel_hits + n_sct_hits < 2) continue;

    track_number++;
    track_E_total += track_particle.e();


    double track_flightDirRelEta = NAN;
    if (!std::isnan(jf_phi)) {
      TVector3 track_flightDirRelVect = track_particle.p4().Vect();
      if (track_flightDirRelVect.Perp()) {
        track_flightDirRelVect.SetTheta(track_flightDirRelVect.Angle(flightDir));
        track_flightDirRelEta = track_flightDirRelVect.PseudoRapidity();
      }
    }

    track_flightDirRelEta_total += track_flightDirRelEta;
    if(std::isnan(min_track_flightDirRelEta) || track_flightDirRelEta < min_track_flightDirRelEta){
      min_track_flightDirRelEta = track_flightDirRelEta;
    }
    if (std::isnan(max_track_flightDirRelEta) || track_flightDirRelEta > max_track_flightDirRelEta) {
      max_track_flightDirRelEta = track_flightDirRelEta;
    }
    if (secondary_jf_vtx_index >= 0) {
      for (const ElementLink<xAOD::TrackParticleContainer>& vertex_track_particle : (**m_jf_vertices(btag).at(secondary_jf_vtx_index)).track_links()) {
        if (*vertex_track_particle == &track_particle) {
          secondaryVtx_charge+=track_particle.charge(); // calculate the total charge
          secondaryVtx_track_number++;
          TLorentzVector track_fourVector;
          track_fourVector.SetVectM(track_particle.p4().Vect(), track_mass);
          secondaryVtx_4momentum_total += track_fourVector;
          secondaryVtx_4momentum_vector.push_back(track_fourVector);
          secondaryVtx_charge_vector.push_back(track_particle.charge());
          secondaryVtx_track_flightDirRelEta_total += track_flightDirRelEta;
          if (std::isnan(secondaryVtx_min_track_flightDirRelEta) || track_flightDirRelEta < secondaryVtx_min_track_flightDirRelEta) {
            secondaryVtx_min_track_flightDirRelEta = track_flightDirRelEta;
          }
          if (std::isnan(secondaryVtx_max_track_flightDirRelEta) || track_flightDirRelEta > secondaryVtx_max_track_flightDirRelEta) {
            secondaryVtx_max_track_flightDirRelEta = track_flightDirRelEta;
          }
        }
      }
    }
  }

  m_secondaryVtx_nTrks(btag) = secondaryVtx_track_number;

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
      EFrac = E / track_E_total;
    }
    m_secondaryVtx_m(btag) = m;
    m_secondaryVtx_E(btag) = E;
    m_secondaryVtx_EFrac(btag) = EFrac;
    m_secondaryVtx_DmesonMass(btag) = m;
    m_secondaryVtx_isDmesonRecon(btag) = false;

    // now reconstruct D mesons
    if(secondaryVtx_track_number == 3 && abs(secondaryVtx_charge) == 1){ // D+ -> K- pi+ pi+, and charge conjugate
      std::vector<TLorentzVector>::const_iterator secondaryVtx_4momentum_Iter = secondaryVtx_4momentum_vector.begin();
      for (std::vector<float>::const_iterator secondaryVtx_charge_Iter = secondaryVtx_charge_vector.begin();
      secondaryVtx_charge_Iter != secondaryVtx_charge_vector.end();
      ++secondaryVtx_charge_Iter) {
        TLorentzVector track_fourVector_new;
        if((secondaryVtx_charge==1 && (*secondaryVtx_charge_Iter)==-1) || (secondaryVtx_charge==-1 && (*secondaryVtx_charge_Iter)==1)){
          track_fourVector_new.SetVectM((*secondaryVtx_4momentum_Iter).Vect(), track_kaon);
        } else {
          track_fourVector_new.SetVectM((*secondaryVtx_4momentum_Iter).Vect(), track_mass);
        }
        secondaryVtx_4momentum_Dmeson += track_fourVector_new;
        ++secondaryVtx_4momentum_Iter;
      }
      if(secondaryVtx_4momentum_Dmeson.M()>1000 && secondaryVtx_4momentum_Dmeson.M()<2200){
        m_secondaryVtx_DmesonMass(btag) = secondaryVtx_4momentum_Dmeson.M();
        m_secondaryVtx_isDmesonRecon(btag) = true;
      }
    } else if(secondaryVtx_track_number == 2 && secondaryVtx_charge == 0){ // D0 -> K- pi+
      std::vector<TLorentzVector>::const_iterator secondaryVtx_4momentum_Iter = secondaryVtx_4momentum_vector.begin();
      for (std::vector<float>::const_iterator secondaryVtx_charge_Iter = secondaryVtx_charge_vector.begin();
      secondaryVtx_charge_Iter != secondaryVtx_charge_vector.end();
      ++secondaryVtx_charge_Iter) {
        TLorentzVector track_fourVector_new;
        if((*secondaryVtx_charge_Iter)==-1){
          track_fourVector_new.SetVectM((*secondaryVtx_4momentum_Iter).Vect(), track_kaon);
        } else {
          track_fourVector_new.SetVectM((*secondaryVtx_4momentum_Iter).Vect(), track_mass);
        }
        secondaryVtx_4momentum_Dmeson += track_fourVector_new;
        ++secondaryVtx_4momentum_Iter;
      }
      if(secondaryVtx_4momentum_Dmeson.M()>1000 && secondaryVtx_4momentum_Dmeson.M()<2200){
        m_secondaryVtx_DmesonMass(btag) = secondaryVtx_4momentum_Dmeson.M();
        m_secondaryVtx_isDmesonRecon(btag) = true;
      }
    } else if(secondaryVtx_track_number == 4 && secondaryVtx_charge == 0){ // D0 -> K- pi+ pi- pi+
      std::vector<TLorentzVector>::const_iterator secondaryVtx_4momentum_Iter = secondaryVtx_4momentum_vector.begin();
      int KaonFlag = -1; //if Kaon is already taken: KaonFlag = +1
      for (std::vector<float>::const_iterator secondaryVtx_charge_Iter = secondaryVtx_charge_vector.begin();
      secondaryVtx_charge_Iter != secondaryVtx_charge_vector.end();
      ++secondaryVtx_charge_Iter) {
        TLorentzVector track_fourVector_new;
        if((*secondaryVtx_charge_Iter)==-1 && KaonFlag == -1){
          track_fourVector_new.SetVectM((*secondaryVtx_4momentum_Iter).Vect(), track_kaon);
          KaonFlag=1;
        } else {
          track_fourVector_new.SetVectM((*secondaryVtx_4momentum_Iter).Vect(), track_mass);
        }
        secondaryVtx_4momentum_Dmeson += track_fourVector_new;
        ++secondaryVtx_4momentum_Iter;
      }
      double mDmeson_1 = secondaryVtx_4momentum_Dmeson.M();
      secondaryVtx_4momentum_Iter = secondaryVtx_4momentum_vector.begin();
      KaonFlag = -1;
      TLorentzVector secondaryVtx_4momentum_Dmeson_new;
      for(std::vector<float>::const_iterator secondaryVtx_charge_Iter = secondaryVtx_charge_vector.begin();
      secondaryVtx_charge_Iter != secondaryVtx_charge_vector.end();
      ++secondaryVtx_charge_Iter) {
        TLorentzVector track_fourVector_new2;
        if((*secondaryVtx_charge_Iter)==-1 && KaonFlag == -1){
          KaonFlag=1;
        } else if ((*secondaryVtx_charge_Iter)==-1 && KaonFlag == 1){
          track_fourVector_new2.SetVectM((*secondaryVtx_4momentum_Iter).Vect(), track_kaon);
        } else{
          track_fourVector_new2.SetVectM((*secondaryVtx_4momentum_Iter).Vect(), track_mass);
        }
        secondaryVtx_4momentum_Dmeson_new += track_fourVector_new2;
        ++secondaryVtx_4momentum_Iter;
      } 
      double mDmeson_2 = secondaryVtx_4momentum_Dmeson_new.M();
      if (abs(mDmeson_1 - 1864.83)<=abs(mDmeson_2 - 1864.83) && mDmeson_1>1000 && mDmeson_1<2200){
          m_secondaryVtx_DmesonMass(btag)=mDmeson_1;
          m_secondaryVtx_isDmesonRecon(btag) = true;
      }
      else if(abs(mDmeson_2 - 1864.83)<=abs(mDmeson_1 - 1864.83) && mDmeson_2>1000 && mDmeson_2<2200){
          m_secondaryVtx_DmesonMass(btag)=mDmeson_2; 
          m_secondaryVtx_isDmesonRecon(btag) = true;
      }
    }
  }
  if(secondaryVtx_track_number > 0){
    double min = secondaryVtx_min_track_flightDirRelEta;
    double max = secondaryVtx_max_track_flightDirRelEta;
    double avg = secondaryVtx_track_flightDirRelEta_total / secondaryVtx_track_number;
    m_secondaryVtx_min_trk_flightDirRelEta(btag) = min;
    m_secondaryVtx_max_trk_flightDirRelEta(btag) = max;
    m_secondaryVtx_avg_trk_flightDirRelEta(btag) = avg;
  }else{
    m_secondaryVtx_min_trk_flightDirRelEta(btag) = NAN;
    m_secondaryVtx_max_trk_flightDirRelEta(btag) = NAN;
    m_secondaryVtx_avg_trk_flightDirRelEta(btag) = NAN;
  }
  if(track_number > 0){
    double min = min_track_flightDirRelEta;
    double max = max_track_flightDirRelEta;
    double avg = track_flightDirRelEta_total / track_number;
    m_min_trk_flightDirRelEta(btag) = min;
    m_max_trk_flightDirRelEta(btag) = max;
    m_avg_trk_flightDirRelEta(btag) = avg;
  }else{
    m_min_trk_flightDirRelEta(btag) = NAN;
    m_max_trk_flightDirRelEta(btag) = NAN;
    m_avg_trk_flightDirRelEta(btag) = NAN;
  }

  m_rnnip_isDefaults(btag) = 0;

}


bool BTagJetAugmenter::jfIsDefaults(const xAOD::BTagging& btag) {
  return !(m_jf_vertices(btag).size() > 0 && (m_jf_nVtx(btag) > 0 || m_jf_nSingleTracks(btag) > 0));
}
