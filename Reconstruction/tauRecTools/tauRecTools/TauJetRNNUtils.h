/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUJETRNNUTILS_H
#define TAUREC_TAUJETRNNUTILS_H

#include <unordered_map>

#include "AsgMessaging/AsgMessaging.h"

#include "xAODTau/TauJet.h"


namespace TauJetRNNUtils {

/**
 * @brief Tool to calculate input variables for the RNN-based tau identification
 *
 *   Used to calculate input variables of the RNN-based tau identification on
 *   the fly by providing a mapping between variable names (strings) and
 *   functions to calculate these variables.
 *
 * @author C. Deutsch
 * @author W. Davey
 *
 */
class VarCalc : public asg::AsgMessaging {
public:
    // Pointers to calculator functions
    using ScalarCalc = bool (*)(const xAOD::TauJet &, double &);

    using TrackCalc = bool (*)(const xAOD::TauJet &, const xAOD::TauTrack &,
                               double &);

    using ClusterCalc = bool (*)(const xAOD::TauJet &,
                                 const xAOD::CaloCluster &, double &);

public:
    VarCalc();
    ~VarCalc() = default;

    // Methods to compute the output (vector) based on the variable name

    // Computes high-level ID variables
    bool compute(const std::string &name, const xAOD::TauJet &tau, double &out) const;

    // Computes track variables
    bool compute(const std::string &name, const xAOD::TauJet &tau,
                 const std::vector<const xAOD::TauTrack *> &tracks,
                 std::vector<double> &out) const;

    // Computes cluster variables
    bool compute(const std::string &name, const xAOD::TauJet &tau,
                 const std::vector<const xAOD::CaloCluster *> &clusters,
                 std::vector<double> &out) const;

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

// Factory function to create a variable calculator populated with default
// variables
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

bool pt(const xAOD::TauJet &tau, double &out);

bool ptDetectorAxis(const xAOD::TauJet &tau, double &out);

bool ptIntermediateAxis(const xAOD::TauJet &tau, double &out);

//functions to calculate input variables needed for the eVeto RNN
bool ptJetSeed_log             (const xAOD::TauJet &tau, double &out);
bool absleadTrackEta           (const xAOD::TauJet &tau, double &out);
bool leadTrackDeltaEta         (const xAOD::TauJet &tau, double &out);
bool leadTrackDeltaPhi         (const xAOD::TauJet &tau, double &out);
bool EMFracFixed               (const xAOD::TauJet &tau, double &out);
bool etHotShotWinOverPtLeadTrk (const xAOD::TauJet &tau, double &out);
bool hadLeakFracFixed          (const xAOD::TauJet &tau, double &out);
bool PSFrac                    (const xAOD::TauJet &tau, double &out);
bool ClustersMeanCenterLambda  (const xAOD::TauJet &tau, double &out);
bool ClustersMeanEMProbability (const xAOD::TauJet &tau, double &out);
bool ClustersMeanFirstEngDens  (const xAOD::TauJet &tau, double &out);
bool ClustersMeanPresamplerFrac(const xAOD::TauJet &tau, double &out);
bool ClustersMeanSecondLambda  (const xAOD::TauJet &tau, double &out);
bool EMPOverTrkSysP            (const xAOD::TauJet &tau, double &out);


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

bool nInnermostPixelHits(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool nPixelHits(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool nSCTHits(
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

// trigger variants
bool nIBLHitsAndExp (
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool nPixelHitsPlusDeadSensors (
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool nSCTHitsPlusDeadSensors (
    const xAOD::TauJet &tau, const xAOD::TauTrack &track, double &out);

bool eProbabilityHT(
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

bool SECOND_LAMBDAOverClustersMeanSecondLambda   (const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);
bool CENTER_LAMBDAOverClustersMeanCenterLambda   (const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);
bool FirstEngDensOverClustersMeanFirstEngDens    (const xAOD::TauJet &tau, const xAOD::CaloCluster &cluster, double &out);

} // namespace Cluster
} // namespace Variables
} // namespace TauJetRNNUtils

#endif // TAUREC_TAUJETRNNUTILS_H
