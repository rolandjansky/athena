/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauJetRNNUtils.h"

#define GeV 1000

namespace TauJetRNNUtils {

VarCalc::VarCalc() : asg::AsgMessaging("TauJetRNNUtils::VarCalc") {
}

bool VarCalc::compute(const std::string &name, const xAOD::TauJet &tau,
                      double &out) {
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
                      std::vector<double> &out) {
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
    for (const auto trk : tracks) {
        success = success && func(tau, *trk, value);
        out.push_back(value);
    }

    return success;
}

bool VarCalc::compute(const std::string &name, const xAOD::TauJet &tau,
                      const std::vector<const xAOD::CaloCluster *> &clusters,
                      std::vector<double> &out) {
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
    for (const auto cls : clusters) {
        success = success && func(tau, *cls, value);
        out.push_back(value);
    }

    return success;
}

void VarCalc::insert(const std::string &name, ScalarCalc func) {
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_scalar_map[name] = func;
}

void VarCalc::insert(const std::string &name, TrackCalc func) {
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_track_map[name] = func;
}

void VarCalc::insert(const std::string &name, ClusterCalc func) {
    if (!func) {
        throw std::invalid_argument("Nullptr passed to VarCalc::insert");
    }
    m_cluster_map[name] = func;
}

std::unique_ptr<VarCalc> get_default_calculator() {
    auto calc = std::make_unique<VarCalc>();

    // Scalar variable calculator functions
    calc->insert("centFrac", Variables::centFrac);
    calc->insert("etOverPtLeadTrk", Variables::etOverPtLeadTrk);
    calc->insert("innerTrkAvgDist", Variables::innerTrkAvgDist);
    calc->insert("absipSigLeadTrk", Variables::absipSigLeadTrk);
    calc->insert("SumPtTrkFrac", Variables::SumPtTrkFrac);
    calc->insert("EMPOverTrkSysP", Variables::EMPOverTrkSysP);
    calc->insert("ptRatioEflowApprox", Variables::ptRatioEflowApprox);
    calc->insert("mEflowApprox", Variables::mEflowApprox);
    calc->insert("dRmax", Variables::dRmax);
    calc->insert("trFlightPathSig", Variables::trFlightPathSig);
    calc->insert("massTrkSys", Variables::massTrkSys);
    calc->insert("pt", Variables::pt);
    calc->insert("ptDetectorAxis", Variables::ptDetectorAxis);
    calc->insert("ptIntermediateAxis", Variables::ptIntermediateAxis);

    // Track variable calculator functions
    calc->insert("pt_log", Variables::Track::pt_log);
    calc->insert("pt_jetseed_log", Variables::Track::pt_jetseed_log);
    calc->insert("d0_abs_log", Variables::Track::d0_abs_log);
    calc->insert("z0sinThetaTJVA_abs_log", Variables::Track::z0sinThetaTJVA_abs_log);
    calc->insert("dEta", Variables::Track::dEta);
    calc->insert("dPhi", Variables::Track::dPhi);
    calc->insert("nInnermostPixelHits", Variables::Track::nInnermostPixelHits);
    calc->insert("nPixelHits", Variables::Track::nPixelHits);
    calc->insert("nSCTHits", Variables::Track::nSCTHits);
    calc->insert("nIBLHitsAndExp", Variables::Track::nIBLHitsAndExp);
    calc->insert("nPixelHitsPlusDeadSensors", Variables::Track::nPixelHitsPlusDeadSensors);
    calc->insert("nSCTHitsPlusDeadSensors", Variables::Track::nSCTHitsPlusDeadSensors);

    // Cluster variable calculator functions
    calc->insert("et_log", Variables::Cluster::et_log);
    calc->insert("pt_jetseed_log", Variables::Cluster::pt_jetseed_log);
    calc->insert("dEta", Variables::Cluster::dEta);
    calc->insert("dPhi", Variables::Cluster::dPhi);
    calc->insert("SECOND_R", Variables::Cluster::SECOND_R);
    calc->insert("SECOND_LAMBDA", Variables::Cluster::SECOND_LAMBDA);
    calc->insert("CENTER_LAMBDA", Variables::Cluster::CENTER_LAMBDA);

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
    const auto success = tau.detail(TauDetail::etOverPtLeadTrk,
                                    etOverPtLeadTrk);
    out = TMath::Log10(std::max(etOverPtLeadTrk, 0.1f));
    return success;
}

bool innerTrkAvgDist(const xAOD::TauJet &tau, double &out) {
    float innerTrkAvgDist;
    const auto success = tau.detail(TauDetail::innerTrkAvgDist,
                                    innerTrkAvgDist);
    out = innerTrkAvgDist;
    return success;
}

bool absipSigLeadTrk(const xAOD::TauJet &tau, double &out) {
    float ipSigLeadTrk;
    const auto success = tau.detail(TauDetail::ipSigLeadTrk, ipSigLeadTrk);
    out = std::min(TMath::Abs(ipSigLeadTrk), 30.0f);
    return success;
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
    out = TMath::Log10(std::max(EMPOverTrkSysP, 1e-3f));
    return success;
}

bool ptRatioEflowApprox(const xAOD::TauJet &tau, double &out) {
    float ptRatioEflowApprox;
    const auto success = tau.detail(TauDetail::ptRatioEflowApprox,
                                    ptRatioEflowApprox);
    out = std::min(ptRatioEflowApprox, 4.0f);
    return success;
}

bool mEflowApprox(const xAOD::TauJet &tau, double &out) {
    float mEflowApprox;
    const auto success = tau.detail(TauDetail::mEflowApprox, mEflowApprox);
    out = TMath::Log10(std::max(mEflowApprox, 140.0f));
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
    const auto success = tau.detail(TauDetail::trFlightPathSig,
                                    trFlightPathSig);
    out = TMath::Log10(std::max(trFlightPathSig, 0.01f));
    return success;
}

bool massTrkSys(const xAOD::TauJet &tau, double &out) {
    float massTrkSys;
    const auto success = tau.detail(TauDetail::massTrkSys, massTrkSys);
    out = TMath::Log10(std::max(massTrkSys, 140.0f));
    return success;
}

bool pt(const xAOD::TauJet &tau, double &out) {
    out = TMath::Log10(std::min(tau.pt() / GeV, 100.0));
    return true;
}

bool ptDetectorAxis(const xAOD::TauJet &tau, double &out) {
    out = TMath::Log10(std::min(tau.ptDetectorAxis() / GeV, 100.0));
    return true;
}

bool ptIntermediateAxis(const xAOD::TauJet &tau, double &out) {
    out = TMath::Log10(std::min(tau.ptIntermediateAxis() /GeV, 100.0));
    return true;
}


namespace Track {

bool pt_log(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
            double &out) {
    out = TMath::Log10(track.pt());
    return true;
}

bool pt_jetseed_log(const xAOD::TauJet &tau, const xAOD::TauTrack& /*track*/,
                    double &out) {
    out = TMath::Log10(tau.ptJetSeed());
    return true;
}

bool d0_abs_log(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
                double &out) {
    out = TMath::Log10(TMath::Abs(track.track()->d0()) + 1e-6);
    return true;
}

bool z0sinThetaTJVA_abs_log(const xAOD::TauJet& tau, const xAOD::TauTrack &track,
                            double &out) {
    out = TMath::Log10(TMath::Abs(track.z0sinThetaTJVA(tau)) + 1e-6);
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

bool nInnermostPixelHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
                         double &out) {
    uint8_t inner_pixel_hits;
    const auto success = track.track()->summaryValue(
        inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);
    out = inner_pixel_hits;
    return success;
}

bool nPixelHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
                double &out) {
    uint8_t pixel_hits;
    const auto success = track.track()->summaryValue(
        pixel_hits, xAOD::numberOfPixelHits);
    out = pixel_hits;
    return success;
}

bool nSCTHits(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
              double &out) {
    uint8_t sct_hits;
    const auto success = track.track()->summaryValue(
        sct_hits, xAOD::numberOfSCTHits);
    out = sct_hits;
    return success;
}

// same as in tau track classification for trigger
bool nIBLHitsAndExp(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
		    double &out) {
    uint8_t inner_pixel_hits, inner_pixel_exp;
    const auto success1 = track.track()->summaryValue(inner_pixel_hits, xAOD::numberOfInnermostPixelLayerHits);
    const auto success2 = track.track()->summaryValue(inner_pixel_exp, xAOD::expectInnermostPixelLayerHit);
    out =  inner_pixel_exp ? inner_pixel_hits : 1.;
    return success1 && success2;
}

bool nPixelHitsPlusDeadSensors(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
			       double &out) {
    uint8_t pixel_hits, pixel_dead;
    const auto success1 = track.track()->summaryValue(pixel_hits, xAOD::numberOfPixelHits);
    const auto success2 = track.track()->summaryValue(pixel_dead, xAOD::numberOfPixelDeadSensors);
    out = pixel_hits + pixel_dead;
    return success1 && success2;
}

bool nSCTHitsPlusDeadSensors(const xAOD::TauJet& /*tau*/, const xAOD::TauTrack &track,
			     double &out) {
    uint8_t sct_hits, sct_dead;
    const auto success1 = track.track()->summaryValue(sct_hits, xAOD::numberOfSCTHits);
    const auto success2 = track.track()->summaryValue(sct_dead, xAOD::numberOfSCTDeadSensors);
    out = sct_hits + sct_dead;
    return success1 && success2;
}

} // namespace Track


namespace Cluster {
using MomentType = xAOD::CaloCluster::MomentType;

bool et_log(const xAOD::TauJet& /*tau*/, const xAOD::CaloCluster &cluster,
            double &out) {
    out = TMath::Log10(cluster.et());
    return true;
}

bool pt_jetseed_log(const xAOD::TauJet &tau, const xAOD::CaloCluster& /*cluster*/,
                    double &out) {
    out = TMath::Log10(tau.ptJetSeed());
    return true;
}

bool dEta(const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster,
          double &out) {
    out = cluster.eta() - tau.eta();
    return true;
}

bool dPhi(const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster,
          double &out) {
    out = cluster.p4().DeltaPhi(tau.p4());
    return true;
}

bool SECOND_R(const xAOD::TauJet& /*tau*/, const xAOD::CaloCluster &cluster,
              double &out) {
    const auto success = cluster.retrieveMoment(MomentType::SECOND_R, out);
    out = TMath::Log10(out + 0.1);
    return success;
}

bool SECOND_LAMBDA(const xAOD::TauJet& /*tau*/, const xAOD::CaloCluster &cluster,
                   double &out) {
    const auto success = cluster.retrieveMoment(MomentType::SECOND_LAMBDA, out);
    out = TMath::Log10(out + 0.1);
    return success;
}

bool CENTER_LAMBDA(const xAOD::TauJet& /*tau*/, const xAOD::CaloCluster &cluster,
                   double &out) {
    const auto success = cluster.retrieveMoment(MomentType::CENTER_LAMBDA, out);
    out = TMath::Log10(out + 1e-6);
    return success;
}

} // namespace Cluster
} // namespace Variables
} // namespace TauJetRNNUtils
