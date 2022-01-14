/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibFitters/CurvedPatRec.h"

#include <TString.h>  // for Form

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibFitters/CurvedCandidateFinder.h"
#include "MuonCalibMath/Combination.h"
#include "cmath"
#include "time.h"
using namespace MuonCalib;
CurvedPatRec::CurvedPatRec() = default;

CurvedPatRec::CurvedPatRec(const double &road_width) { m_road_width = road_width; }

double CurvedPatRec::roadWidth() const { return m_road_width; }
void CurvedPatRec::setRoadWidth(const double &r_road_width) { m_road_width = r_road_width; }
void CurvedPatRec::setTimeOut(const double &time_out) { m_time_out = time_out; }
bool CurvedPatRec::fit(MuonCalibSegment &r_segment) const {
    // select all hits //
    HitSelection selection(r_segment.mdtHitsOnTrack(), 0);
    // call the other fit function //
    return fit(r_segment, selection);
}
bool CurvedPatRec::fit(MuonCalibSegment &r_segment, HitSelection r_selection) const {
    CurvedLine curved_track;
    return fit(r_segment, r_selection, curved_track);
}
bool CurvedPatRec::fit(MuonCalibSegment &r_segment, HitSelection r_selection, CurvedLine &curved_track) const {
    ///////////////
    // VARIABLES //
    ///////////////
    std::unique_ptr<StraightPatRec> sfitter = std::make_unique<StraightPatRec>();
    time_t start, end;  // start and end times (needed for time-out)
    double diff;        // difference of start and end time (needed for time-out)
    Combination combination;
    std::vector<unsigned int> hit_index;  // hit indices for a given combination
    unsigned int try_nb_hits;             // try to find a segment with try_nb_hits hits
    bool segment_found(false);            // flag indicating the a segment has been found
    MdtHitVec cand_track_hits;            // vector of the track hits
                                          // found so far
    CurvedLine aux_line;                  // memory for reconstructed curved lines
    Amg::Vector3D null(0.0, 0.0, 0.0);
    Amg::Vector3D xhat(1.0, 0.0, 0.0);
    std::vector<Amg::Vector3D> points;  // hit points for the track fit
    MdtHitVec loc_track_hits;           // track hit store

    ////////////
    // RESETS //
    ////////////
    time(&start);

    ////////////////////////////////////////
    // CHECK SIZE OF THE SELECTION VECTOR //
    ////////////////////////////////////////

    if (r_selection.size() != r_segment.mdtHitsOnTrack()) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nCurvedPatRec::fit - Size of selection vector does not match the number of hits on track!", __FILE__,
                 __LINE__));
    }

    //////////////////////
    // PREPARATORY WORK //
    //////////////////////

    // perform a straight track fit to get an estimate of the incidence angle //
    Amg::Vector3D est_dir(0.0, 0.0, 1.0);
    sfitter->setRoadWidth(2.0 * m_road_width);
    sfitter->setTimeOut(0.5 * m_time_out);
    MTStraightLine track;
    if (sfitter->fit(r_segment, r_selection, track)) { est_dir = track.directionVector(); }

    // store track hits //
    unsigned int k = 0;
    for (const MuonCalibSegment::MdtHitPtr &hit : r_segment.mdtHOT()) {
        if (r_selection[k] == 0 && hit->sigmaDriftRadius() < 100) { loc_track_hits.push_back(hit); };
        ++k;
    }

    // return, if there are too few hits //
    if (loc_track_hits.size() < 4) { return false; }
    /////////////////////////
    // PATTERN RECOGNITION //
    /////////////////////////

    // try to find a segment with as many hits on it as possible //
    try_nb_hits = loc_track_hits.size();

    MdtHitVec stored_track_hits;
    double chi2 = -1.;

    while (!segment_found && try_nb_hits > 3) {
        // loop over the combinations //
        combination.setNewParameters(loc_track_hits.size(), try_nb_hits);
        for (unsigned int cb = 0; cb < combination.numberOfCombinations(); cb++) {
            // time-out //
            time(&end);
            diff = difftime(end, start);
            if (diff > m_time_out) {
                MsgStream log(Athena::getMessageSvc(), "CurvedPatRec");
                log << MSG::WARNING << "Class CurvedPatRec, method fit: time-out for track finding after " << m_time_out << " seconds!"
                    << endmsg;
                return false;
            }

            // analyse the hit combination //
            if (cb == 0) {
                combination.currentCombination(hit_index);
            } else {
                combination.nextCombination(hit_index);
            }
            MdtHitVec track_hits;
            for (unsigned int k = 0; k < try_nb_hits; ++k) { track_hits.push_back(loc_track_hits[hit_index[k] - 1]); }

            // find candidates //
            CurvedCandidateFinder finder(track_hits);
            const std::vector<CurvedLine> candidates(finder.getCandidates(m_road_width, est_dir));
            if (candidates.empty()) { continue; }

            segment_found = true;

            for (unsigned int cand = 0; cand < candidates.size(); cand++) {
                std::vector<Amg::Vector3D> errors(track_hits.size());
                for (unsigned int k = 0; k < errors.size(); k++) {
                    if (track_hits[k]->sigmaDriftRadius() > 0.0) {
                        errors[k] = Amg::Vector3D(1.0, track_hits[k]->sigmaDriftRadius(), 0.0);
                    } else {
                        errors[k] = Amg::Vector3D(1.0, 1.0, 0.0);
                    }
                }

                // get hit points //
                points = getHitPoints(track_hits, candidates[cand]);

                // fit a curved line through the points //
                aux_line = CurvedLine(points, errors);

                // calculate chi^2 //
                double tmp_chi2(0.0);
                for (unsigned int k = 0; k < track_hits.size(); k++) {
                    MTStraightLine tang(curved_track.getTangent((track_hits[k]->localPosition()).z()));
                    MTStraightLine wire(Amg::Vector3D(0.0, track_hits[k]->localPosition().y(), track_hits[k]->localPosition().z()), xhat,
                                        null, null);
                    double d(std::abs(tang.signDistFrom(wire)));
                    if (track_hits[k]->sigma2DriftRadius() != 0) {
                        tmp_chi2 = tmp_chi2 + std::pow(d - track_hits[k]->driftRadius(), 2) / track_hits[k]->sigma2DriftRadius();
                    } else {
                        tmp_chi2 = tmp_chi2 + std::pow(d - track_hits[k]->driftRadius(), 2) / 0.01;
                    }
                }

                // compare chi^2 with chi^2 values found so far //
                if (chi2 < 0 || tmp_chi2 < chi2) {
                    chi2 = tmp_chi2;
                    curved_track = aux_line;
                    // store the track hits //
                    stored_track_hits = track_hits;
                }
            }
        }

        try_nb_hits = try_nb_hits - 1;
    }

    if (!segment_found) { return false; }

    ///////////////////////////////
    // SECOND REFINED CURVED FIT //
    ///////////////////////////////

    // get hit points //
    points = getHitPoints(stored_track_hits, curved_track);
    std::vector<Amg::Vector3D> errors(stored_track_hits.size());
    for (unsigned int k = 0; k < errors.size(); k++) {
        if (stored_track_hits[k]->sigmaDriftRadius() > 0.0) {
            errors[k] = Amg::Vector3D(1.0, stored_track_hits[k]->sigmaDriftRadius(), 0.0);
        } else {
            errors[k] = Amg::Vector3D(1.0, 1.0, 0.0);
        }
    }

    // fit a curved line through the points //
    curved_track = CurvedLine(points, errors);

    /////////////////////
    // CALCULATE CHI^2 //
    /////////////////////

    chi2 = 0.0;
    for (unsigned int k = 0; k < stored_track_hits.size(); k++) {
        MTStraightLine tang(curved_track.getTangent((stored_track_hits[k]->localPosition()).z()));
        MTStraightLine wire(Amg::Vector3D(0.0, stored_track_hits[k]->localPosition().y(), stored_track_hits[k]->localPosition().z()), xhat,
                            null, null);
        double d(std::abs(tang.signDistFrom(wire)));
        if (stored_track_hits[k]->sigma2DriftRadius() != 0) {
            chi2 += std::pow(d - stored_track_hits[k]->driftRadius(), 2) / stored_track_hits[k]->sigma2DriftRadius();
        } else {
            chi2 += std::pow(d - stored_track_hits[k]->driftRadius(), 2) / 0.01;
        }
    }

    //////////////////////////
    // UPDATE HIT RESIDUALS //
    //////////////////////////

    for (const MdtHitPtr& hit : r_segment.mdtHOT()) {
        Amg::Vector3D pos(0.0, hit->localPosition().y(), hit->localPosition().z());
        MTStraightLine aux_line(pos, xhat, null, null);

        MTStraightLine tang(curved_track.getTangent(pos.z()));

        double dist(tang.signDistFrom(aux_line));  // track distance
        double dist_err(1.0);                      // unknown error of the track distance
        hit->setDistanceToTrack(dist, dist_err);
    }

    if (std::isnan(chi2)) { chi2 = 1.0e6; }

    ///////////////////////////////////////////////
    // UPDATE SEGMENT POSITION, DIRECTION, CHI^2 //
    ///////////////////////////////////////////////

    MTStraightLine tangent(curved_track.getTangent((r_segment.position()).z()));
    r_segment.set(chi2 / (stored_track_hits.size() - 3), tangent.positionVector(), tangent.directionVector());
    curved_track.setChi2(chi2);
    curved_track.setNumberOfTrackHits(stored_track_hits.size());
    curved_track.setUsedHits(stored_track_hits);
    return true;
}

Amg::Vector3D CurvedPatRec::getHitPoint(const MdtHitPtr &hit, const MTStraightLine &straight_track) const {
    /////////////////////////
    // CALCULATE HIT POINT //
    /////////////////////////

    Amg::Vector3D point = straight_track.positionVector() +
                          (straight_track.directionVector().unit().dot(hit->localPosition() - straight_track.positionVector())) *
                              straight_track.directionVector().unit();
    Amg::Vector3D point_2 = hit->localPosition() + hit->driftRadius() * (point - hit->localPosition()).unit();

    return point_2;
}

std::vector<Amg::Vector3D> CurvedPatRec::getHitPoints(const MdtHitVec &track_hits, const MTStraightLine &straight_track) const {
    ///////////////
    // VARIABLES //
    ///////////////

    std::vector<Amg::Vector3D> hit_vec;

    /////////////////////
    // FILL HIT VECTOR //
    /////////////////////

    for (unsigned int k = 0; k < track_hits.size(); k++) { hit_vec.emplace_back(getHitPoint(track_hits[k], straight_track)); }

    ///////////////////////////
    // RETURN THE HIT VECTOR //
    ///////////////////////////

    return hit_vec;
}

std::vector<Amg::Vector3D> CurvedPatRec::getHitPoints(const MdtHitVec &track_hits, const CurvedLine &curved_track) const {
    ///////////////
    // VARIABLES //
    ///////////////

    std::vector<Amg::Vector3D> hit_vec;

    /////////////////////
    // FILL HIT VECTOR //
    /////////////////////

    for (unsigned int k = 0; k < track_hits.size(); k++) {
        hit_vec.emplace_back(getHitPoint(track_hits[k], curved_track.getTangent((track_hits[k]->localPosition()).z())));
    }

    ///////////////////////////
    // RETURN THE HIT VECTOR //
    ///////////////////////////

    return hit_vec;
}
