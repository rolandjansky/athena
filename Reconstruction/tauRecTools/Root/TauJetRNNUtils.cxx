/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauJetRNNUtils.h"
#include "tauRecTools/HelperFunctions.h"
#include <algorithm>
#define GeV 1000

namespace TauJetRNNUtils {

VarCalc::VarCalc() : asg::AsgMessaging("TauJetRNNUtils::VarCalc") {
}

bool VarCalc::compute(const std::string &name, const xAOD::TauJet &tau,
                      double &out) const {
    // Retrieve calculator function
    ScalarCalc func = nullptr;
    try {
        func = m_scalar_map.at(name);
    } catch (const std::out_of_range &e) {
        ATH_MSG_ERROR("Variable '" << name << "' not defined");
        throw;
    }

    // Calculate variable
    return func(tau, out);
}

bool VarCalc::compute(const std::string &name, const xAOD::TauJet &tau,
                      const std::vector<const xAOD::TauTrack *> &tracks,
                      std::vector<double> &out) const {
    out.clear();

    // Retrieve calculator function
    TrackCalc func = nullptr;
    try {
        func = m_track_map.at(name);
    } catch (const std::out_of_range &e) {
        ATH_MSG_ERROR("Variable '" << name << "' not defined");
        throw;
    }

    // Calculate variables for selected tracks
    bool success = true;
    double value;
    for (const auto *const trk : tracks) {
        success = success && func(tau, *trk, value);
        out.push_back(value);
    }

    return success;
}

bool VarCalc::compute(const std::string &name, const xAOD::TauJet &tau,
                      const std::vector<xAOD::CaloVertexedTopoCluster> &clusters,
                      std::vector<double> &out) const {
    out.clear();

    // Retrieve calculator function
    ClusterCalc func = nullptr;
    try {
        func = m_cluster_map.at(name);
    } catch (const std::out_of_range &e) {
        ATH_MSG_ERROR("Variable '" << name << "' not defined");
        throw;
    }

    // Calculate variables for selected clusters
    bool success = true;
    double value;
    for (const xAOD::CaloVertexedTopoCluster& cluster : clusters) {
        success = success && func(tau, cluster, value);
        out.push_back(value);
    }

    return success;
}

void VarCalc::insert(const std::string &name, ScalarCalc func, const std::vector<std::string>& scalar_vars) {
    if (std::find(scalar_vars.begin(), scalar_vars.end(), name) == scalar_vars.end()) {
      return;
    }
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_scalar_map[name] = func;
}

void VarCalc::insert(const std::string &name, TrackCalc func, const std::vector<std::string>& track_vars) {
    if (std::find(track_vars.begin(), track_vars.end(), name) == track_vars.end()) {
      return;
    }
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_track_map[name] = func;
}

void VarCalc::insert(const std::string &name, ClusterCalc func, const std::vector<std::string>& cluster_vars) {
    if (std::find(cluster_vars.begin(), cluster_vars.end(), name) == cluster_vars.end()) {
      return;
    }
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_cluster_map[name] = func;
}

std::unique_ptr<VarCalc> get_calculator(const std::vector<std::string>& scalar_vars,
					const std::vector<std::string>& track_vars,
					const std::vector<std::string>& cluster_vars) {
    auto calc = std::make_unique<VarCalc>();

    // Scalar variable calculator functions
    calc->insert("centFrac", Variables::centFrac, scalar_vars);
    calc->insert("etOverPtLeadTrk", Variables::etOverPtLeadTrk, scalar_vars);
    calc->insert("innerTrkAvgDist", Variables::innerTrkAvgDist, scalar_vars);
    calc->insert("absipSigLeadTrk", Variables::absipSigLeadTrk, scalar_vars);
    calc->insert("SumPtTrkFrac", Variables::SumPtTrkFrac, scalar_vars);
    calc->insert("EMPOverTrkSysP", Variables::EMPOverTrkSysP, scalar_vars);
    calc->insert("ptRatioEflowApprox", Variables::ptRatioEflowApprox, scalar_vars);
    calc->insert("mEflowApprox", Variables::mEflowApprox, scalar_vars);
    calc->insert("dRmax", Variables::dRmax, scalar_vars);
    calc->insert("trFlightPathSig", Variables::trFlightPathSig, scalar_vars);
    calc->insert("massTrkSys", Variables::massTrkSys, scalar_vars);
    calc->insert("pt", Variables::pt, scalar_vars);
    calc->insert("pt_tau_log", Variables::pt_tau_log, scalar_vars);
    calc->insert("ptDetectorAxis", Variables::ptDetectorAxis, scalar_vars);
    calc->insert("ptIntermediateAxis", Variables::ptIntermediateAxis, scalar_vars);
    //---added for the eVeto
    calc->insert("ptJetSeed_log",              Variables::ptJetSeed_log, scalar_vars);
    calc->insert("absleadTrackEta",            Variables::absleadTrackEta, scalar_vars);
    calc->insert("leadTrackDeltaEta",          Variables::leadTrackDeltaEta, scalar_vars);
    calc->insert("leadTrackDeltaPhi",          Variables::leadTrackDeltaPhi, scalar_vars);
    calc->insert("leadTrackProbNNorHT",        Variables::leadTrackProbNNorHT, scalar_vars);
    calc->insert("EMFracFixed",                Variables::EMFracFixed, scalar_vars);
    calc->insert("etHotShotWinOverPtLeadTrk",  Variables::etHotShotWinOverPtLeadTrk, scalar_vars);
    calc->insert("hadLeakFracFixed",           Variables::hadLeakFracFixed, scalar_vars);
    calc->insert("PSFrac",                     Variables::PSFrac, scalar_vars);
    calc->insert("ClustersMeanCenterLambda",   Variables::ClustersMeanCenterLambda, scalar_vars);
    calc->insert("ClustersMeanFirstEngDens",   Variables::ClustersMeanFirstEngDens, scalar_vars);
    calc->insert("ClustersMeanPresamplerFrac", Variables::ClustersMeanPresamplerFrac, scalar_vars);

    // Track variable calculator functions
    calc->insert("pt_log", Variables::Track::pt_log, track_vars);
    calc->insert("pt_tau_log", Variables::Track::pt_tau_log, track_vars);
    calc->insert("pt_jetseed_log", Variables::Track::pt_jetseed_log, track_vars);
    calc->insert("d0_abs_log", Variables::Track::d0_abs_log, track_vars);
    calc->insert("z0sinThetaTJVA_abs_log", Variables::Track::z0sinThetaTJVA_abs_log, track_vars);
    calc->insert("z0sinthetaTJVA", Variables::Track::z0sinthetaTJVA, track_vars);
    calc->insert("z0sinthetaSigTJVA", Variables::Track::z0sinthetaSigTJVA, track_vars);
    calc->insert("d0TJVA", Variables::Track::d0TJVA, track_vars);
    calc->insert("d0SigTJVA", Variables::Track::d0SigTJVA, track_vars);
    calc->insert("dEta", Variables::Track::dEta, track_vars);
    calc->insert("dPhi", Variables::Track::dPhi, track_vars);
    calc->insert("nInnermostPixelHits", Variables::Track::nInnermostPixelHits, track_vars);
    calc->insert("nPixelHits", Variables::Track::nPixelHits, track_vars);
    calc->insert("nSCTHits", Variables::Track::nSCTHits, track_vars);
    calc->insert("nIBLHitsAndExp", Variables::Track::nIBLHitsAndExp, track_vars);
    calc->insert("nPixelHitsPlusDeadSensors", Variables::Track::nPixelHitsPlusDeadSensors, track_vars);
    calc->insert("nSCTHitsPlusDeadSensors", Variables::Track::nSCTHitsPlusDeadSensors, track_vars);
    calc->insert("eProbabilityHT", Variables::Track::eProbabilityHT, track_vars);
    calc->insert("eProbabilityNN", Variables::Track::eProbabilityNN, track_vars);
    calc->insert("eProbabilityNNorHT", Variables::Track::eProbabilityNNorHT, track_vars);

    // Cluster variable calculator functions
    calc->insert("et_log", Variables::Cluster::et_log, cluster_vars);
    calc->insert("pt_tau_log", Variables::Cluster::pt_tau_log, cluster_vars);
    calc->insert("pt_jetseed_log", Variables::Cluster::pt_jetseed_log, cluster_vars);
    calc->insert("dEta", Variables::Cluster::dEta, cluster_vars);
    calc->insert("dPhi", Variables::Cluster::dPhi, cluster_vars);
    calc->insert("SECOND_R", Variables::Cluster::SECOND_R, cluster_vars);
    calc->insert("SECOND_LAMBDA", Variables::Cluster::SECOND_LAMBDA, cluster_vars);
    calc->insert("CENTER_LAMBDA", Variables::Cluster::CENTER_LAMBDA, cluster_vars);
    //---added for the eVeto
    calc->insert("SECOND_LAMBDAOverClustersMeanSecondLambda", Variables::Cluster::SECOND_LAMBDAOverClustersMeanSecondLambda, cluster_vars);
    calc->insert("CENTER_LAMBDAOverClustersMeanCenterLambda", Variables::Cluster::CENTER_LAMBDAOverClustersMeanCenterLambda, cluster_vars);
    calc->insert("FirstEngDensOverClustersMeanFirstEngDens" , Variables::Cluster::FirstEngDensOverClustersMeanFirstEngDens, cluster_vars);

    return calc;
}


namespace Variables {
using TauDetail = xAOD::TauJetParameters::Detail;

bool centFrac(const xAOD::TauJet &tau, double &out) {
    float centFrac;
    const auto success = tau.detail(TauDetail::centFrac, centFrac);
    out = std::min(centFrac, 1.0f);
    return success;
}

bool etOverPtLeadTrk(const xAOD::TauJet &tau, double &out) {
    float etOverPtLeadTrk;
    const auto success = tau.detail(TauDetail::etOverPtLeadTrk, etOverPtLeadTrk);
    out = std::log10(std::max(etOverPtLeadTrk, 0.1f));
    return success;
}

bool innerTrkAvgDist(const xAOD::TauJet &tau, double &out) {
    float innerTrkAvgDist;
    const auto success = tau.detail(TauDetail::innerTrkAvgDist, innerTrkAvgDist);
    out = innerTrkAvgDist;
    return success;
}

bool absipSigLeadTrk(const xAOD::TauJet &tau, double &out) {
    float ipSigLeadTrk = (tau.nTracks()>0) ? tau.track(0)->d0SigTJVA() : 0.;
    out = std::min(std::abs(ipSigLeadTrk), 30.0f);
    return true;
}

bool SumPtTrkFrac(const xAOD::TauJet &tau, double &out) {
    float SumPtTrkFrac;
    const auto success = tau.detail(TauDetail::SumPtTrkFrac, SumPtTrkFrac);
    out = SumPtTrkFrac;
    return success;
}

bool EMPOverTrkSysP(const xAOD::TauJet &tau, double &out) {
    float EMPOverTrkSysP;
    const auto success = tau.detail(TauDetail::EMPOverTrkSysP, EMPOverTrkSysP);
    out = std::log10(std::max(EMPOverTrkSysP, 1e-3f));
    return success;
}

bool ptRatioEflowApprox(const xAOD::TauJet &tau, double &out) {
    float ptRatioEflowApprox;
    const auto success = tau.detail(TauDetail::ptRatioEflowApprox, ptRatioEflowApprox);
    out = std::min(ptRatioEflowApprox, 4.0f);
    return success;
}

bool mEflowApprox(const xAOD::TauJet &tau, double &out) {
    float mEflowApprox;
    const auto success = tau.detail(TauDetail::mEflowApprox, mEflowApprox);
    out = std::log10(std::max(mEflowApprox, 140.0f));
    return success;
}

bool dRmax(const xAOD::TauJet &tau, double &out) {
    float dRmax;
    const auto success = tau.detail(TauDetail::dRmax, dRmax);
    out = dRmax;
    return success;
}

bool trFlightPathSig(const xAOD::TauJet &tau, double &out) {
    float trFlightPathSig;
    const auto success = tau.detail(TauDetail::trFlightPathSig, trFlightPathSig);
    out = std::log10(std::max(trFlightPathSig, 0.01f));
    return success;
}

bool massTrkSys(const xAOD::TauJet &tau, double &out) {
    float massTrkSys;
    const auto success = tau.detail(TauDetail::massTrkSys, massTrkSys);
    out = std::log10(std::max(massTrkSys, 140.0f));
    return success;
}

bool pt(const xAOD::TauJet &tau, double &out) {
    out = std::log10(std::min(tau.pt() / GeV, 100.0));
    return true;
}

bool pt_tau_log(const xAOD::TauJet &tau, double &out) {
    out = std::log10(std::max(tau.pt() / GeV, 1e-6));
    return true;
}

bool ptDetectorAxis(const xAOD::TauJet &tau, double &out) {
    out = std::log10(std::min(tau.ptDetectorAxis() / GeV, 100.0));
    return true;
}

bool ptIntermediateAxis(const xAOD::TauJet &tau, double &out) {
    out = std::log10(std::min(tau.ptIntermediateAxis() /GeV, 100.0));
    return true;
}

bool ptJetSeed_log(const xAOD::TauJet &tau, double &out) {
  out = std::log10(std::max(tau.ptJetSeed(), 1e-3));
  return true;
}

bool absleadTrackEta(const xAOD::TauJet &tau, double &out){
  static const SG::AuxElement::ConstAccessor<float> acc_absEtaLeadTrack("ABS_ETA_LEAD_TRACK");
  float absEtaLeadTrack = acc_absEtaLeadTrack(tau);
  out = std::max(0.f, absEtaLeadTrack);
  return true;
}

bool leadTrackDeltaEta(const xAOD::TauJet &tau, double &out){
  static const SG::AuxElement::ConstAccessor<float> acc_absDeltaEta("TAU_ABSDELTAETA");
  float absDeltaEta = acc_absDeltaEta(tau);
  out = std::max(0.f, absDeltaEta);
  return true;
}

bool leadTrackDeltaPhi(const xAOD::TauJet &tau, double &out){
  static const SG::AuxElement::ConstAccessor<float> acc_absDeltaPhi("TAU_ABSDELTAPHI");
  float absDeltaPhi = acc_absDeltaPhi(tau);
  out = std::max(0.f, absDeltaPhi);
  return true;
}

bool leadTrackProbNNorHT(const xAOD::TauJet &tau, double &out){
  auto tracks = tau.allTracks();

  // Sort tracks in descending pt order
  if (!tracks.empty()) {
    auto cmp_pt = [](const xAOD::TauTrack *lhs, const xAOD::TauTrack *rhs) {
      return lhs->pt() > rhs->pt();
    };
    std::sort(tracks.begin(), tracks.end(), cmp_pt);

    const xAOD::TauTrack* tauLeadTrack = tracks.at(0);
    const xAOD::TrackParticle* xTrackParticle = tauLeadTrack->track();
    float eProbabilityHT = xTrackParticle->summaryValue(eProbabilityHT, xAOD::eProbabilityHT);
    static const SG::AuxElement::ConstAccessor<float> acc_eProbabilityNN("eProbabilityNN");
    float eProbabilityNN = acc_eProbabilityNN(*xTrackParticle);
    out = (tauLeadTrack->pt()>2000.) ? eProbabilityNN : eProbabilityHT;
  }
  else {
    out = 0.;
  }
  return true;
}

bool EMFracFixed(const xAOD::TauJet &tau, double &out){
  static const SG::AuxElement::ConstAccessor<float> acc_emFracFixed("EMFracFixed");
  float emFracFixed = acc_emFracFixed(tau);
  out = std::max(emFracFixed, 0.0f);
  return true;
}

bool etHotShotWinOverPtLeadTrk(const xAOD::TauJet &tau, double &out){
  static const SG::AuxElement::ConstAccessor<float> acc_etHotShotWinOverPtLeadTrk("etHotShotWinOverPtLeadTrk");
  float etHotShotWinOverPtLeadTrk = acc_etHotShotWinOverPtLeadTrk(tau);
  out = std::max(etHotShotWinOverPtLeadTrk, 1e-6f);
  out = std::log10(out);
  return true;
}

bool hadLeakFracFixed(const xAOD::TauJet &tau, double &out){
  static const SG::AuxElement::ConstAccessor<float> acc_hadLeakFracFixed("hadLeakFracFixed");
  float hadLeakFracFixed = acc_hadLeakFracFixed(tau);
  out = std::max(0.f, hadLeakFracFixed);
  return true;
}

bool PSFrac(const xAOD::TauJet &tau, double &out){
  float PSFrac;
  const auto success = tau.detail(TauDetail::PSSFraction, PSFrac);
  out = std::max(0.f,PSFrac);  
  return success;
}

bool ClustersMeanCenterLambda(const xAOD::TauJet &tau, double &out){
  float ClustersMeanCenterLambda;
  const auto success = tau.detail(TauDetail::ClustersMeanCenterLambda, ClustersMeanCenterLambda);
  out = std::max(0.f, ClustersMeanCenterLambda);
  return success;
}

bool ClustersMeanEMProbability(const xAOD::TauJet &tau, double &out){
  float ClustersMeanEMProbability;
  const auto success = tau.detail(TauDetail::ClustersMeanEMProbability, ClustersMeanEMProbability);
  out = std::max(0.f, ClustersMeanEMProbability);
  return success;
}

bool ClustersMeanFirstEngDens(const xAOD::TauJet &tau, double &out){
  float ClustersMeanFirstEngDens;
  const auto success = tau.detail(TauDetail::ClustersMeanFirstEngDens, ClustersMeanFirstEngDens);
  out =  std::max(-10.f, ClustersMeanFirstEngDens);
  return success;
}

bool ClustersMeanPresamplerFrac(const xAOD::TauJet &tau, double &out){
  float ClustersMeanPresamplerFrac;
  const auto success = tau.detail(TauDetail::ClustersMeanPresamplerFrac, ClustersMeanPresamplerFrac);
  out = std::max(0.f, ClustersMeanPresamplerFrac);
  return success;
}

bool ClustersMeanSecondLambda(const xAOD::TauJet &tau, double &out){
  float ClustersMeanSecondLambda;
  const auto success = tau.detail(TauDetail::ClustersMeanSecondLambda, ClustersMeanSecondLambda);
  out = std::max(0.f, ClustersMeanSecondLambda);
  return success;
}

namespace Track {

bool pt_log(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    out = std::log10(track.pt());
    return true;
}

bool pt_tau_log(const xAOD::TauJet &tau, const xAOD::TauTrack& /*track*/, double &out) {
    out = std::log10(std::max(tau.pt(), 1e-6));
    return true;
}

bool pt_jetseed_log(const xAOD::TauJet &tau, const xAOD::TauTrack& /*track*/, double &out) {
    out = std::log10(tau.ptJetSeed());
    return true;
}

bool d0_abs_log(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    out = std::log10(std::abs(track.d0TJVA()) + 1e-6);
    return true;
}

bool z0sinThetaTJVA_abs_log(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    out = std::log10(std::abs(track.z0sinthetaTJVA()) + 1e-6);
    return true;
}

bool z0sinthetaTJVA(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    out = track.z0sinthetaTJVA();
    return true;
}

bool z0sinthetaSigTJVA(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    out = track.z0sinthetaSigTJVA();
    return true;
}

bool d0TJVA(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    out = track.d0TJVA();
    return true;
}

bool d0SigTJVA(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    out = track.d0SigTJVA();
    return true;
}

bool dEta(const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out) {
    out = track.eta() - tau.eta();
    return true;
}

bool dPhi(const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out) {
    out = track.p4().DeltaPhi(tau.p4());
    return true;
}

bool nInnermostPixelHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    uint8_t inner_pixel_hits;
    const auto success = track.track()->summaryValue(inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);
    out = inner_pixel_hits;
    return success;
}

bool nPixelHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    uint8_t pixel_hits;
    const auto success = track.track()->summaryValue(pixel_hits, xAOD::numberOfPixelHits);
    out = pixel_hits;
    return success;
}

bool nSCTHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    uint8_t sct_hits;
    const auto success = track.track()->summaryValue(sct_hits, xAOD::numberOfSCTHits);
    out = sct_hits;
    return success;
}

// same as in tau track classification for trigger
bool nIBLHitsAndExp(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    uint8_t inner_pixel_hits, inner_pixel_exp;
    const auto success1 = track.track()->summaryValue(inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);
    const auto success2 = track.track()->summaryValue(inner_pixel_exp, xAOD::expectInnermostPixelLayerHit);
    out =  inner_pixel_exp ? inner_pixel_hits : 1.;
    return success1 && success2;
}

bool nPixelHitsPlusDeadSensors(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    uint8_t pixel_hits, pixel_dead;
    const auto success1 = track.track()->summaryValue(pixel_hits, xAOD::numberOfPixelHits);
    const auto success2 = track.track()->summaryValue(pixel_dead, xAOD::numberOfPixelDeadSensors);
    out = pixel_hits + pixel_dead;
    return success1 && success2;
}

bool nSCTHitsPlusDeadSensors(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    uint8_t sct_hits, sct_dead;
    const auto success1 = track.track()->summaryValue(sct_hits, xAOD::numberOfSCTHits);
    const auto success2 = track.track()->summaryValue(sct_dead, xAOD::numberOfSCTDeadSensors);
    out = sct_hits + sct_dead;
    return success1 && success2;
}

bool eProbabilityHT(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {
    float eProbabilityHT;
    const auto success = track.track()->summaryValue(eProbabilityHT, xAOD::eProbabilityHT);
    out = eProbabilityHT;
    return success;
}

bool eProbabilityNN(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {  
    static const SG::AuxElement::ConstAccessor<float> acc_eProbabilityNN("eProbabilityNN");
    out = acc_eProbabilityNN(track);
    return true;
}

bool eProbabilityNNorHT(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track, double &out) {  
  auto atrack = track.track();
  float eProbabilityHT = atrack->summaryValue(eProbabilityHT, xAOD::eProbabilityHT);
  static const SG::AuxElement::ConstAccessor<float> acc_eProbabilityNN("eProbabilityNN");
  float eProbabilityNN = acc_eProbabilityNN(*atrack);
  out = (atrack->pt()>2000.) ? eProbabilityNN : eProbabilityHT;
  return true;
}

} // namespace Track


namespace Cluster {
using MomentType = xAOD::CaloCluster::MomentType;

bool et_log(const xAOD::TauJet& /*tau*/, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
    out = std::log10(cluster.p4().Et());
    return true;
}

bool pt_tau_log(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster& /*cluster*/, double &out) {
    out = std::log10(std::max(tau.pt(), 1e-6));
    return true;
}

bool pt_jetseed_log(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster& /*cluster*/, double &out) {
    out = std::log10(tau.ptJetSeed());
    return true;
}

bool dEta(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
    out = cluster.eta() - tau.eta();
    return true;
}

bool dPhi(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
    out = cluster.p4().DeltaPhi(tau.p4());
    return true;
}

bool SECOND_R(const xAOD::TauJet& /*tau*/, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
    const auto success = cluster.clust().retrieveMoment(MomentType::SECOND_R, out);
    out = std::log10(out + 0.1);
    return success;
}

bool SECOND_LAMBDA(const xAOD::TauJet& /*tau*/, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
    const auto success = cluster.clust().retrieveMoment(MomentType::SECOND_LAMBDA, out);
    out = std::log10(out + 0.1);
    return success;
}

bool CENTER_LAMBDA(const xAOD::TauJet& /*tau*/, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
    const auto success = cluster.clust().retrieveMoment(MomentType::CENTER_LAMBDA, out);
    out = std::log10(out + 1e-6);
    return success;
}

bool SECOND_LAMBDAOverClustersMeanSecondLambda(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
  static const SG::AuxElement::ConstAccessor<float> acc_ClustersMeanSecondLambda("ClustersMeanSecondLambda");
  float ClustersMeanSecondLambda = acc_ClustersMeanSecondLambda(tau);
  double secondLambda(0);
  const auto success = cluster.clust().retrieveMoment(MomentType::SECOND_LAMBDA, secondLambda);
  out = (ClustersMeanSecondLambda != 0.) ? secondLambda/ClustersMeanSecondLambda : 0.;
  return success;
}

bool CENTER_LAMBDAOverClustersMeanCenterLambda(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
  static const SG::AuxElement::ConstAccessor<float> acc_ClustersMeanCenterLambda("ClustersMeanCenterLambda");
  float ClustersMeanCenterLambda = acc_ClustersMeanCenterLambda(tau);
  double centerLambda(0);
  const auto success = cluster.clust().retrieveMoment(MomentType::CENTER_LAMBDA, centerLambda);
  if (ClustersMeanCenterLambda == 0.){
    out = 250.;
  }else {
    out = centerLambda/ClustersMeanCenterLambda;
  }

  out = std::min(out, 250.);

  return success;
}


bool FirstEngDensOverClustersMeanFirstEngDens(const xAOD::TauJet &tau, const xAOD::CaloVertexedTopoCluster &cluster, double &out) {
  // the ClustersMeanFirstEngDens is the log10 of the energy weighted average of the First_ENG_DENS 
  // divided by ETot to make it dimension-less, 
  // so we need to evaluate the difference of log10(clusterFirstEngDens/clusterTotalEnergy) and the ClustersMeanFirstEngDens
  double clusterFirstEngDens = 0.0;
  bool status = cluster.clust().retrieveMoment(MomentType::FIRST_ENG_DENS, clusterFirstEngDens);
  if (clusterFirstEngDens < 1e-6) clusterFirstEngDens = 1e-6;

  static const SG::AuxElement::ConstAccessor<float> acc_ClusterTotalEnergy("ClusterTotalEnergy");
  float clusterTotalEnergy = acc_ClusterTotalEnergy(tau);
  if (clusterTotalEnergy < 1e-6) clusterTotalEnergy = 1e-6;

  static const SG::AuxElement::ConstAccessor<float> acc_ClustersMeanFirstEngDens("ClustersMeanFirstEngDens");
  float clustersMeanFirstEngDens = acc_ClustersMeanFirstEngDens(tau);

  out = std::log10(clusterFirstEngDens/clusterTotalEnergy) - clustersMeanFirstEngDens;
  
  return status;
}

} // namespace Cluster
} // namespace Variables
} // namespace TauJetRNNUtils
