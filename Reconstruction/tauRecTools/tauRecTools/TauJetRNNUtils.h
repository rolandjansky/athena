#ifndef TAURECTOOLS_TAUJETRNNUTILS_H
#define TAURECTOOLS_TAUJETRNNUTILS_H

#include <unordered_map>

#include "xAODTau/TauJet.h"


namespace TauJetRNNUtils {

// String-based lookup of variables to calculate them on-the-fly
class VarCalc {
public:
    // Functional signature of the calculator functions
    using ScalarCalc = bool (*)(const xAOD::TauJet &, double &);

    using TrackCalc = bool (*)(const xAOD::TauJet &, const xAOD::TauTrack &,
                               double &);

    using ClusterCalc = bool (*)(const xAOD::TauJet &,
                                 const xAOD::CaloCluster &, double &);

public:
    VarCalc() = default;
    ~VarCalc() = default;

    // Methods to compute the output (vector) based on variable name
    bool compute(const std::string &name, const xAOD::TauJet &tau, double &out);

    bool compute(const std::string &name, const xAOD::TauJet &tau,
                 const std::vector<const xAOD::TauTrack *> &tracks,
                 std::vector<double> &out);

    bool compute(const std::string &name, const xAOD::TauJet &tau,
                 const std::vector<const xAOD::CaloCluster *> &clusters,
                 std::vector<double> &out);

    // Methods to insert calculator functions into the lookup table
    void insert(const std::string &name, ScalarCalc func);
    void insert(const std::string &name, TrackCalc func);
    void insert(const std::string &name, ClusterCalc func);

private:
    // Lookup tables
    std::unordered_map<std::string, ScalarCalc> m_scalar_map;
    std::unordered_map<std::string, TrackCalc> m_track_map;
    std::unordered_map<std::string, ClusterCalc> m_cluster_map;
};


std::unique_ptr<VarCalc> get_default_calculator();


namespace Variables {

// Functions to calculate (scalar) input variables
// Returns a status code indicating success

bool centFrac(const xAOD::TauJet &tau, double &out);

bool etOverPtLeadTrk(const xAOD::TauJet &tau, double &out);

bool innerTrkAvgDist(const xAOD::TauJet &tau, double &out);

bool absipSigLeadTrk(const xAOD::TauJet &tau, double &out);

bool SumPtTrkFrac(const xAOD::TauJet &tau, double &out);

bool EMPOverTrkSysP(const xAOD::TauJet &tau, double &out);

bool ptRatioEflowApprox(const xAOD::TauJet &tau, double &out);

bool mEflowApprox(const xAOD::TauJet &tau, double &out);

bool dRmax(const xAOD::TauJet &tau, double &out);

bool trFlightPathSig(const xAOD::TauJet &tau, double &out);

bool massTrkSys(const xAOD::TauJet &tau, double &out);

bool ptIntermediateAxis(const xAOD::TauJet &tau, double &out);


namespace Track {

// Functions to calculate input variables for each track
// Returns a status code indicating success

bool pt_log(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool pt_jetseed_log(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool d0_abs_log(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool z0sinThetaTJVA_abs_log(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool dEta(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool dPhi(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool eProbabilityHT(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool nInnermostPixelHits(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool nPixelHits(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool nSCTHits(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

} // namespace Track

namespace Cluster {

// Functions to calculate input variables for each cluster
// Returns a status code indicating success

bool et_log(
    const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);

bool pt_jetseed_log(
    const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);

bool dEta(
    const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);

bool dPhi(
    const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);

bool SECOND_R(
    const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);

bool SECOND_LAMBDA(
    const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);

bool CENTER_LAMBDA(
    const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);

} // namespace Cluster
} // namespace Variables
} // namespace TauJetRNNUtils

#endif // TAURECTOOLS_TAUJETRNNUTILS_H
