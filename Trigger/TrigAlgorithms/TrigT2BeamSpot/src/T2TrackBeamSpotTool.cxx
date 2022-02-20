/*
  Copyright (C) 2020, 2022 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <sstream>
#include <cmath>

// This algorithm
#include "T2TrackBeamSpotTool.h"

// Specific to this algorithm
#include "T2Track.h"
#include "TrkParameters/TrackParameters.h"
#include "T2TrackBSLLPoly.h"

// Generic Trigger tools
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//Conversion units
#include "GaudiKernel/SystemOfUnits.h"

using Gaudi::Units::GeV;
using Gaudi::Units::mm;


using namespace PESA;

namespace {


// BCID is in the range 0 to NUM_BCID-1 (inclusive)
unsigned const NUM_BCID = 3564;

/*
* This struct wraps a special monitored array which holds elements of
* matrices used by LS fitter. Array elements need to be mapped to
* individual histogram bins (have to use "kVec" option) and summed
* across whole farm.
*/
struct MonitoredLSMatrices {

    // update with a single track
    void update(T2TrackBeamSpotTool::TrackData const& seed, double beamSize);

    void publish(ToolHandle<GenericMonitoringTool> const& monTool);

    std::vector<double> beamLSMatrices;  // 16 elements total
    std::vector<double> beamLSMatricesBCID;  // 8 * NUM_BCID elements

};

// update with a single track
void MonitoredLSMatrices::update(T2TrackBeamSpotTool::TrackData const& seed, double beamSize) {

    unsigned const sizeBCID = 2*(2+1)/2 + 2 + 2 + 1;

    if (beamLSMatrices.empty()) {
        // reserve space for elements of all matrices:
        // - 4x4 symmetrical matrix
        // - 4-element vector
        // - 1 sum of d0 squares
        // - 1 track count
        beamLSMatrices.resize(4*(4+1)/2 + 4 + 1 + 2 + 1);
        // per-BCID needs space for:
        // - 2x2 symmetrical matrix
        // - 2-element vector
        beamLSMatricesBCID.resize(sizeBCID * NUM_BCID);
    }

    double const sin_phi0 = std::sin(seed.phi0);
    double const cos_phi0 = std::cos(seed.phi0);
    double const param[] = { -sin_phi0, cos_phi0, -seed.z0*sin_phi0, seed.z0*cos_phi0};
    double d0_var = seed.d0_var + beamSize*beamSize;
    int idx = 0;
    for (int i = 0; i < 4; ++ i) {
        for (int j = 0; j <= i; ++ j) {
            beamLSMatrices[idx++] += param[i]*param[j] / d0_var;
        }
    }
    for (int i = 0; i < 4; ++ i) {
        beamLSMatrices[idx++] += seed.d0*param[i] / d0_var;
    }
    beamLSMatrices[idx++] += seed.d0*seed.d0 / d0_var;
    beamLSMatrices[idx++] += seed.z0;
    beamLSMatrices[idx++] += seed.z0*seed.z0;
    beamLSMatrices[idx++] += 1;

    idx = sizeBCID * seed.bcid;
    beamLSMatricesBCID[idx++] += param[0]*param[0] / d0_var;
    beamLSMatricesBCID[idx++] += param[0]*param[1] / d0_var;
    beamLSMatricesBCID[idx++] += param[1]*param[1] / d0_var;
    beamLSMatricesBCID[idx++] += seed.d0*param[0] / d0_var;
    beamLSMatricesBCID[idx++] += seed.d0*param[1] / d0_var;
    beamLSMatricesBCID[idx++] += seed.z0;
    beamLSMatricesBCID[idx++] += seed.z0*seed.z0;
    beamLSMatricesBCID[idx++] += 1;
}

void MonitoredLSMatrices::publish(ToolHandle<GenericMonitoringTool> const& monTool)
{
    if (not beamLSMatrices.empty()) {
        auto matrices = Monitored::Collection("BeamLSMatrices", beamLSMatrices);
        auto matricesBCID = Monitored::Collection("BeamLSMatricesBCID", beamLSMatricesBCID);
        auto mon = Monitored::Group(monTool, matrices, matricesBCID);
    }
}

} // namespace



T2TrackBeamSpotTool::T2TrackBeamSpotTool( const std::string& type, const std::string& name, const IInterface* parent )
    : AthAlgTool(type, name, parent)
{
    declareProperty("doLeastSquares",  m_doLeastSquares   = true);
    declareProperty("doLogLikelihood", m_doLogLikelihood  = true);
    declareProperty("beamSizeLS",      m_beamSizeLS       =   0.01*mm);
}

StatusCode T2TrackBeamSpotTool::initialize()
{
    ATH_MSG_INFO("Initialising T2TrackBeamSpotTool tool");

    // Retrieve tools
    ATH_CHECK(m_trackFilterTool.retrieve());
    if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());

    return StatusCode::SUCCESS;
}

void T2TrackBeamSpotTool::updateBS(const TrackCollection& tracks, EventIDBase const& eventID) const
{
    //Select tracks
    auto selectedTracks = m_trackFilterTool->filter(tracks);

    std::vector<TrackData> bsTracks;
    bool has_bs = m_trackFilterTool->updateBS(selectedTracks, eventID.lumi_block(),
                                              eventID.bunch_crossing_id(), &bsTracks);

    // when beamspot is known we can call actual update methods
    if (has_bs) {
        if (not bsTracks.empty()) {
            updateBS(std::move(bsTracks));
        } else {
            updateBS(m_trackFilterTool->filterBS(selectedTracks), eventID.bunch_crossing_id());
        }
    }
}

void T2TrackBeamSpotTool::updateBS(std::vector<const Trk::Track*>&& tracks, unsigned bcid) const
{
    auto timer = Monitored::Timer("TIME_updateBS");
    if (m_doLeastSquares) {
        ATH_MSG_DEBUG("Fill LS matrices with " << tracks.size() << " Trk::Track tracks");
        MonitoredLSMatrices lsMatrices;
        for (auto& track: tracks) {
            TrackData tdata(*track->perigeeParameters(), 0, bcid);
            lsMatrices.update(tdata, m_beamSizeLS);
        }
        lsMatrices.publish(m_monTool);
    }
    if (m_doLogLikelihood) {
        ATH_MSG_DEBUG("Fill LL coefficients with " << tracks.size() << " Trk::Track tracks");
        std::vector<double> poly_coeff;
        T2TrackBSLLPoly llpoly(m_beamSizeLS);
        for (auto& track: tracks) {
            auto const& par = track->perigeeParameters()->parameters();
            auto const& cov = track->perigeeParameters()->covariance();
            llpoly.update(par[Trk::z0], par[Trk::d0], par[Trk::phi0], (*cov)(Trk::d0, Trk::d0), poly_coeff);
        }
        if (not poly_coeff.empty()) {
            auto coeff = Monitored::Collection("TrackLLPolyCoeff", poly_coeff);
            auto mon = Monitored::Group(m_monTool, coeff);
        }
    }
    auto mon = Monitored::Group(m_monTool, timer);
}

void T2TrackBeamSpotTool::updateBS(std::vector<TrackData>&& tracks) const
{
    auto timer = Monitored::Timer("TIME_updateBS");
    if (m_doLeastSquares) {
        ATH_MSG_DEBUG("Fill LS matrices with " << tracks.size() << " TrackData tracks");
        MonitoredLSMatrices lsMatrices;
        for (auto& track: tracks) {
            lsMatrices.update(track, m_beamSizeLS);
        }
        lsMatrices.publish(m_monTool);
    }
    if (m_doLogLikelihood) {
        ATH_MSG_DEBUG("Fill LL coefficients with " << tracks.size() << " TrackData tracks");
        std::vector<double> poly_coeff;
        T2TrackBSLLPoly llpoly(m_beamSizeLS);
        for (auto& track: tracks) {
            llpoly.update(track.z0, track.d0, track.phi0, track.d0_var, poly_coeff);
        }
        auto coeff = Monitored::Collection("TrackLLPolyCoeff", poly_coeff);
        auto mon = Monitored::Group(m_monTool, coeff);
    }
    auto mon = Monitored::Group(m_monTool, timer);
}
