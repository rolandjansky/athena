/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <sstream>
#include <cmath>

// This algorithm
#include "T2BSTrackFilterTool.h"

// Specific to this algorithm
#include "T2Track.h"
#include "TrkParameters/TrackParameters.h"

// Generic Trigger tools
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//Conversion units
#include "GaudiKernel/SystemOfUnits.h"

using Gaudi::Units::GeV;
using Gaudi::Units::mm;


using namespace PESA;

namespace {

const char* trackRejectReasonName(T2BSTrackFilterTool::TrackRejectReason reason)
{
    switch (reason) {
    case T2BSTrackFilterTool::fail_PT:
        return "PT";
    case T2BSTrackFilterTool::fail_SiHits:
        return "SiHits";
    case T2BSTrackFilterTool::fail_PIXHits:
        return "PIXHits";
    case T2BSTrackFilterTool::fail_SCTHits:
        return "SCTHits";
    case T2BSTrackFilterTool::fail_TRTHits:
        return "TRTHits";
    case T2BSTrackFilterTool::fail_NDF:
        return "NDF";
    case T2BSTrackFilterTool::fail_D0:
        return "D0";
    case T2BSTrackFilterTool::fail_Z0:
        return "Z0";
    case T2BSTrackFilterTool::fail_D0err:
        return "D0err";
    case T2BSTrackFilterTool::fail_Z0err:
        return "Z0err";
    case T2BSTrackFilterTool::fail_Eta:
        return "Eta";
    case T2BSTrackFilterTool::fail_MinQual:
        return "MinQual";
    case T2BSTrackFilterTool::fail_MaxQual:
        return "MaxQual";
    case T2BSTrackFilterTool::fail_Chi2Prob:
        return "Chi2Prob";
    case T2BSTrackFilterTool::fail_D0Chi2:
        return "D0Chi2";
    case T2BSTrackFilterTool::numRejectReasons:
        return "InvalidReason";
    }
    return "InvalidReason";
}

std::string rejectReasonsToString(T2BSTrackFilterTool::TrackRejectReasonSet const& reasons)
{
    std::string str;
    for (int i = 0; i < T2BSTrackFilterTool::numRejectReasons; ++ i) {
        if (reasons.test(i)) {
            if (!str.empty()) str += ',';
            str += trackRejectReasonName(T2BSTrackFilterTool::TrackRejectReason(i));
        }
    }
    return str;
}

} // namespace


T2BSTrackFilterTool::TrackStatAccumulator::TrackStatAccumulator() {
    trk_param.setZero();
    trk_d0.setZero();
}

void
T2BSTrackFilterTool::TrackStatAccumulator::update(T2BSTrackFilterTool::TrackData const& seed, double beamSize) {
    double const sin_phi0 = std::sin(seed.phi0);
    double const cos_phi0 = std::cos(seed.phi0);
    double const param[] = { -sin_phi0, cos_phi0, -seed.z0*sin_phi0, seed.z0*cos_phi0};
    double d0_var = seed.d0_var + beamSize*beamSize;
    for (int i = 0; i < 4; ++ i) {
        for (int j = 0; j < 4; ++ j) {
            trk_param(i, j) += param[i]*param[j] / d0_var;
        }
        trk_d0(i) += seed.d0*param[i] / d0_var;
    }
    ++ trk_count;
    trk_d0_sq += seed.d0*seed.d0 / d0_var;
}

void
T2BSTrackFilterTool::TrackStatAccumulator::update(TrackStatAccumulator const& other) {
    trk_count += other.trk_count;
    trk_d0_sq += other.trk_d0_sq;
    trk_param += other.trk_param;
    trk_d0 += other.trk_d0;
}

// Produce a solution based on current data, can only be called if trk_count > 0
AmgVector(4)
T2BSTrackFilterTool::TrackStatAccumulator::solution() const {
    auto solution = trk_param.inverse()*trk_d0;
    return solution;
}

T2BSTrackFilterTool::T2BSTrackFilterTool( const std::string& type, const std::string& name, const IInterface* parent )
    : AthAlgTool(type, name, parent)
{
    // Track Selection criteria
    declareProperty("TrackMinPt",      m_minTrackPt       =  0.7*GeV );
    declareProperty("TrackMaxEta",     m_maxTrackEta      =   2.5    );
    declareProperty("TrackMaxZ0",      m_maxTrackZ0       = 200.*mm  );
    declareProperty("TrackMaxD0",      m_maxTrackD0       =  10.*mm  );
    declareProperty("TrackMaxZ0err",   m_maxTrackZ0err    =   2.*mm  );
    declareProperty("TrackMaxD0err",   m_maxTrackD0err    =   2.*mm  );
    declareProperty("TrackMinNDF",     m_minTrackNDF      =   2      );
    declareProperty("TrackMinQual",    m_minTrackQual     =   0.2    );
    declareProperty("TrackMaxQual",    m_maxTrackQual     =   5.0    );
    declareProperty("TrackMinChi2Prob",m_minTrackChi2Prob = -10.0    );
    declareProperty("TrackMinSiHits",  m_minSiHits        =   4      );
    declareProperty("TrackMinPIXHits", m_minPIXHits       =   1      );
    declareProperty("TrackMinSCTHits", m_minSCTHits       =   3      );
    declareProperty("TrackMinTRTHits", m_minTRTHits       =   0      );

    // beamspot estimate params
    declareProperty("GoalSeedTracks",  m_goalSeedTracks   =   500    );
    declareProperty("D0Chi2Cutoff",    m_chi2cutoff       =   30     );
    declareProperty("BeamSizeLS",      m_beamSizeLS       =   0.01*mm);
    declareProperty("BSLbnInterval",   m_lbnInterval      =   1      );
}

StatusCode T2BSTrackFilterTool::initialize()
{
    ATH_MSG_INFO("Initialising T2BSTrackFilterTool tool");

    //Retrieve monitoring tool
    ATH_CHECK( m_monTool.retrieve());

    m_accumulators.clear();

    return StatusCode::SUCCESS;
}

std::vector<const Trk::Track*>
T2BSTrackFilterTool::filter(const TrackCollection& tracks) const
{
    ATH_MSG_DEBUG( "Selecting tracks for the beamSpot algorithm" );

    //Monitoring counters and timers
    auto timerTrackSelection = Monitored::Timer("TIME_TrackSelection");
    auto nTracksPerCollection           = Monitored::Scalar<unsigned>("TracksPerCollection", 0);
    auto nTracksPassedPerCollection     = Monitored::Scalar<unsigned>("SelectedTracksPerCollection", 0);
    std::vector<unsigned> trackRejectReasonsCounts(numRejectReasons);

    //T2Track with easily accesable parameters
    std::vector<T2Track> myTracks;
    myTracks.reserve(tracks.size());
    std::vector<const Trk::Track*> selectedTracks;
    selectedTracks.reserve(tracks.size());

    // Loop over all tracks in the given track collections
    for (auto trackIter = tracks.begin(); trackIter != tracks.end(); ++trackIter) {

        const Trk::Track& track = **trackIter;

        //Counter for all input tracks
        nTracksPerCollection++;

        const T2Track myTrack(track);
        // Check for passed track
        TrackRejectReasonSet const rejectSet = isGoodTrack(myTrack);
        if (rejectSet.none()) {
            // Add this track to the set used to find a vertex
            selectedTracks.push_back(*trackIter);

            //Stored only for monitoring
            myTracks.push_back(myTrack);

            //Counter for selected tracks
            nTracksPassedPerCollection++;

        } else {
            for (unsigned bit = 0; bit < numRejectReasons; ++ bit) {
                if (rejectSet.test(bit)) {
                    ++trackRejectReasonsCounts[bit];
                }
            }
        }

        ATH_MSG_DEBUG(
                "Track " << (rejectSet.none() ? "passed" : "failed") << " selection:" <<
                " mask: " << rejectReasonsToString(rejectSet) <<
                " d0: " << myTrack.D0() <<
                " z0: " << myTrack.Z0() <<
                " phi0: " << myTrack.Phi() <<
                " eta: " << myTrack.Eta() <<
                " pT: " << myTrack.Pt()*GeV <<
                " Z0err: " << myTrack.Z0err() <<
                " D0err: " << myTrack.D0err() <<
                " Qual: " << myTrack.Qual() <<
                " Chi2Prob: " << myTrack.Chi2Prob() <<
                " NpixSPs: " << myTrack.PIXHits() <<
                " NsctSPs: " << myTrack.SCTHits() <<
                " NstrawHits: " << myTrack.TRTHits());
    } //end for loop over tracks in a collection

    //How many tracks per collection
    ATH_MSG_DEBUG("Total Tracks: " << nTracksPerCollection << " selectedTracks: " << nTracksPassedPerCollection);

    //Monitor all passed tracks variables/parameters
    monitor_tracks("Track", "Pass", myTracks);

    auto rejectReason = Monitored::Collection("TrackRejectReason", trackRejectReasonsCounts);

    //Monitor counters per track collection and time to select tracks
    auto mon = Monitored::Group(m_monTool, nTracksPerCollection, nTracksPassedPerCollection,
                timerTrackSelection, rejectReason);

    return selectedTracks;
}

bool T2BSTrackFilterTool::updateBS(const std::vector<const Trk::Track*>& tracks,
                                   unsigned lbn,  unsigned bcid,
                                   std::vector<TrackData>* bsTracks) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_accumulators.empty()) {
        // no beamspot yet, store track params in the seeds array
        m_bsSeeds.reserve(m_bsSeeds.size() + tracks.size());
        for (auto& track: tracks) {
            m_bsSeeds.emplace_back(*track->perigeeParameters(), lbn, bcid);
        }

        // initial calculation of the beamspot and matrix
        bool has_bs = bootstrapFromSeeds(bsTracks);
        if (has_bs) {
            // can clear seeds
            m_bsSeeds.clear();
        }
        return has_bs;
    }

    // We need to cleanup accumulators for old LBs from time to time,
    // do it when there is a LBN change.
    if (m_accumulators.count(lbn) == 0) {

        // count number of track in all existing accumulators
        unsigned accumTrackCount = std::accumulate(
            m_accumulators.begin(), m_accumulators.end(), 0,
            [](unsigned counter, auto const& pair) { return counter + pair.second.trk_count; }
        );
        ATH_MSG_DEBUG("Currently have " << m_accumulators.size() << " accumulators with "
                      << accumTrackCount << " tracks");

        // only need few recent accumulators to have a reasonable track count, drop
        // some eralier accumulators, for now keep two recent LBNs
        // TODO: need smarter way to decide what to keep
        while (m_accumulators.size() > 2) {
           ATH_MSG_DEBUG("Removing accumulator for LB=" << m_accumulators.begin()->first
                         << " with " << m_accumulators.begin()->second.trk_count << " tracks");
            m_accumulators.erase(m_accumulators.begin());
        }
    }

    // current local solution for beam position
    TrackStatAccumulator acc;
    for (auto&& pair: m_accumulators) {
        acc.update(pair.second);
    }
    auto solution = acc.solution();

    // update already collected statistics, filtering some contributions
    for (auto& track: tracks) {
        auto trackPars = track->perigeeParameters();
        if (trackPars != nullptr) {
            TrackData seed(*trackPars, lbn, bcid);
            double chi2 = trackChi2(seed, solution);
            if (chi2 < m_chi2cutoff) {
                if (msgLvl(MSG::DEBUG)) {
                    dumpTrackParams(trackPars, chi2, "updateBS: track passed chi2 selection");
                }
                m_accumulators[lbn].update(seed, m_beamSizeLS);
            } else {
                if (msgLvl(MSG::DEBUG)) {
                    dumpTrackParams(trackPars, chi2, "updateBS: track failed chi2_d0 restriction");
                }
            }
        }
    }

    return true;
}

std::vector<const Trk::Track*>
T2BSTrackFilterTool::filterBS(const std::vector<const Trk::Track*>& tracks) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    std::vector<const Trk::Track*> selected;
    if (m_accumulators.empty()) {
        // no beamspot yet
        return selected;
    }

    // current local solution for beam position
    TrackStatAccumulator acc;
    for (auto&& pair: m_accumulators) {
        acc.update(pair.second);
    }
    auto solution = acc.solution();

    // update already collected statistics, filtering some contributions
    std::vector<unsigned> trackRejectReasonsCounts(numRejectReasons);
    std::vector<T2Track> t2Tracks;
    t2Tracks.reserve(tracks.size());
    for (auto& track: tracks) {
        auto trackPars = track->perigeeParameters();
        if (trackPars != nullptr) {
            TrackData seed(*trackPars, 0, 0);
            double chi2 = trackChi2(seed, solution);
            if (chi2 < m_chi2cutoff) {
                if (msgLvl(MSG::DEBUG)) {
                    dumpTrackParams(trackPars, chi2, "filterBS: track passed chi2 selection");
                }
                selected.push_back(track);
                t2Tracks.emplace_back(*track);
            } else {
                if (msgLvl(MSG::DEBUG)) {
                    dumpTrackParams(trackPars, chi2, "filterBS: track failed chi2_d0 restriction");
                }
                ++trackRejectReasonsCounts[fail_D0Chi2];
            }
        }
    }

    // monitoring
    auto rejectReason = Monitored::Collection("TrackRejectReason", trackRejectReasonsCounts);

    //Monitor counters per track collection and time to select tracks
    auto mon = Monitored::Group(m_monTool, rejectReason);

    monitor_tracks("Track", "FilterBS", t2Tracks);

    return selected;
}

T2BSTrackFilterTool::TrackRejectReasonSet
T2BSTrackFilterTool::isGoodTrack(const T2Track& track) const
{
    TrackRejectReasonSet reasons;
    if (std::abs(track.Pt()) < m_minTrackPt) reasons.set(fail_PT);
    if (track.SiHits() < m_minSiHits) reasons.set(fail_SiHits);
    if (track.PIXHits() < m_minPIXHits) reasons.set(fail_PIXHits);
    if (track.SCTHits() < m_minSCTHits) reasons.set(fail_SCTHits);
    if (track.TRTHits() < m_minTRTHits) reasons.set(fail_TRTHits);
    if (track.NDF() < m_minTrackNDF) reasons.set(fail_NDF);
    if (std::abs(track.D0()) > m_maxTrackD0) reasons.set(fail_D0);
    if (std::abs(track.Z0()) > m_maxTrackZ0) reasons.set(fail_Z0);
    if (track.D0err() > m_maxTrackD0err) reasons.set(fail_D0err);
    if (track.Z0err() > m_maxTrackZ0err) reasons.set(fail_Z0err);
    if (std::abs(track.Eta()) > m_maxTrackEta) reasons.set(fail_Eta);
    if (track.Qual() < m_minTrackQual) reasons.set(fail_MinQual);
    if (track.Qual() > m_maxTrackQual) reasons.set(fail_MaxQual);
    if (track.Chi2Prob() < m_minTrackChi2Prob) reasons.set(fail_Chi2Prob);
    return reasons;
}

//Monitoring track variables
void T2BSTrackFilterTool::monitor_tracks(std::string const& prefix, std::string const& suffix, std::vector<T2Track> const& tracks) const
{
   auto  trackPt      = Monitored::Collection(  prefix + "Pt"       + suffix, tracks, [](const T2Track& t){ return t.Pt()      ;});
   auto  trackEta     = Monitored::Collection(  prefix + "Eta"      + suffix, tracks, [](const T2Track& t){ return t.Eta()     ;});
   auto  trackPhi     = Monitored::Collection(  prefix + "Phi"      + suffix, tracks, [](const T2Track& t){ return t.Phi()     ;});
   auto  trackZ0      = Monitored::Collection(  prefix + "Z0"       + suffix, tracks, [](const T2Track& t){ return t.Z0()      ;});
   auto  trackD0      = Monitored::Collection(  prefix + "D0"       + suffix, tracks, [](const T2Track& t){ return t.D0()      ;});
   auto  trackZ0err   = Monitored::Collection(  prefix + "Z0err"    + suffix, tracks, [](const T2Track& t){ return t.Z0err()   ;});
   auto  trackD0err   = Monitored::Collection(  prefix + "D0err"    + suffix, tracks, [](const T2Track& t){ return t.D0err()   ;});
   auto  trackNDF     = Monitored::Collection(  prefix + "NDF"      + suffix, tracks, [](const T2Track& t){ return t.NDF()     ;});
   auto  trackQual    = Monitored::Collection(  prefix + "Qual"     + suffix, tracks, [](const T2Track& t){ return t.Qual()    ;});
   auto  trackChi2Prob= Monitored::Collection(  prefix + "Chi2Prob" + suffix, tracks, [](const T2Track& t){ return t.Chi2Prob();});
   auto  trackSiHits  = Monitored::Collection(  prefix + "SiHits"   + suffix, tracks, [](const T2Track& t){ return t.SiHits()  ;});
   auto  trackPiHits  = Monitored::Collection(  prefix + "PIXHits"  + suffix, tracks, [](const T2Track& t){ return t.PIXHits() ;});
   auto  trackSCTHits = Monitored::Collection(  prefix + "SCTHits"  + suffix, tracks, [](const T2Track& t){ return t.SCTHits() ;});
   auto  trackTRTHits = Monitored::Collection(  prefix + "TRTHits"  + suffix, tracks, [](const T2Track& t){ return t.TRTHits() ;});
   auto mon = Monitored::Group(m_monTool, trackPt, trackEta, trackPhi, trackZ0, trackD0, trackZ0err, trackD0err,
                               trackNDF, trackQual, trackChi2Prob, trackSiHits, trackPiHits, trackSCTHits, trackTRTHits);
}

bool T2BSTrackFilterTool::bootstrapFromSeeds(std::vector<TrackData>* bsTracks) const
{
    // needs a number of seeds
    if (m_bsSeeds.size() < m_goalSeedTracks) return false;

    // do all seed track first without filtering
    TrackStatAccumulator acc0;
    for (auto const& seed: m_bsSeeds) {
        acc0.update(seed, m_beamSizeLS);
    }
    auto solution = acc0.solution();
    ATH_MSG_DEBUG("Unfiltered solution=" << solution[0] << ", " << solution[1]
                  << ", " << solution[2] << ", " << solution[3]);
    ATH_MSG_DEBUG("m_trk_count=" << acc0.trk_count);

    // few iterations of the same fit removing outliers
    const int Niter = 2;
    for (int c = 0; c < Niter; ++ c) {
        TrackStatAccumulator acc;

        for (auto const& seed: m_bsSeeds) {
            double chi2 = trackChi2(seed, solution);
            if (chi2 < m_chi2cutoff) {
                acc.update(seed, m_beamSizeLS);
            }
        }
        solution = acc.solution();
        ATH_MSG_DEBUG("Filtered solution [iter " << c << "]=" << solution[0] << ", " << solution[1]
                      << ", " << solution[2] << ", " << solution[3]);
        ATH_MSG_DEBUG("m_trk_count=" << acc.trk_count);
    }

    // final pass
    m_accumulators.clear(); // they should be clear already, just in case
    if (bsTracks != nullptr) {
        // we expects more than half tracks to pass
        bsTracks->reserve(m_bsSeeds.size());
    }
    std::vector<unsigned> trackRejectReasonsCounts(numRejectReasons);
    for (auto const& seed: m_bsSeeds) {
        double chi2 = trackChi2(seed, solution);
        ATH_MSG_DEBUG("chi2=" << chi2);
        if (chi2 < m_chi2cutoff) {
            if (m_accumulators.count(seed.lbn) == 0) {
                ATH_MSG_DEBUG("Adding accumulator for LB=" << seed.lbn);
            }
            m_accumulators[seed.lbn].update(seed, m_beamSizeLS);
            if (bsTracks != nullptr) {
                bsTracks->push_back(seed);
            }
       } else {
            ++trackRejectReasonsCounts[fail_D0Chi2];
        }
    }

    TrackStatAccumulator acc;
    for (auto&& pair: m_accumulators) {
        acc.update(pair.second);
    }
    solution = acc.solution();
    ATH_MSG_DEBUG("Final filtered solution=" << solution[0] << ", " << solution[1]
                  << ", " << solution[2] << ", " << solution[3]);
    ATH_MSG_DEBUG("m_trk_count=" << acc.trk_count);

    // monitoring
    auto rejectReason = Monitored::Collection("TrackRejectReason", trackRejectReasonsCounts);

    //Monitor counters per track collection and time to select tracks
    auto mon = Monitored::Group(m_monTool, rejectReason);

    return true;
}

double T2BSTrackFilterTool::trackChi2(TrackData const& seed, AmgVector(4) const& solution) const {
    double const sin_phi0 = std::sin(seed.phi0);
    double const cos_phi0 = std::cos(seed.phi0);
    double const param[] = {-sin_phi0, cos_phi0, -seed.z0*sin_phi0, seed.z0*cos_phi0};
    double chi2 = seed.d0;
    for (int i = 0; i < 4; ++ i) {
        chi2 -= solution[i]*param[i];
    }
    chi2 = chi2*chi2 / (seed.d0_var + m_beamSizeLS*m_beamSizeLS);
    return chi2;
}

void T2BSTrackFilterTool::dumpTrackParams(Trk::Perigee const* trackPars, double chi2, std::string const& message) const {
    auto&& par = trackPars->parameters();
    auto&& cov = trackPars->covariance();
    if (cov != nullptr) {
        double d0_err = Amg::error(*cov, Trk::d0);
        double phi0_err = Amg::error(*cov, Trk::phi0);
        double z0_err = Amg::error(*cov, Trk::z0);
        double r_est = std::abs((*cov)(Trk::d0, Trk::phi0) / (*cov)(Trk::phi0, Trk::phi0));
        double psi_err = (*cov)(Trk::d0, Trk::d0) / (r_est*r_est) - (*cov)(Trk::phi0, Trk::phi0);
        if (psi_err > 0) {
            psi_err = std::sqrt(psi_err);
        } else {
            psi_err = -std::sqrt(-psi_err);
        }
        ATH_MSG_DEBUG(message << ':'
            << std::setprecision(15)
            << " z0: " << par[Trk::z0]
            << " d0: " << par[Trk::d0]
            << " phi0: " << par[Trk::phi0]
            << " d0_err: " << d0_err
            << " phi0_err: " << phi0_err
            << " d0_phi0_cov: " << (*cov)(Trk::d0, Trk::phi0)
            << " r_est: " << r_est
            << " psi_err: " << psi_err
            << " z0_err: " << z0_err
            << " chi2: " << chi2
            << std::setprecision(6));
    }
}
