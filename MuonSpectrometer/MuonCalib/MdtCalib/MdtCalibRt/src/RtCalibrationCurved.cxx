/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibRt/RtCalibrationCurved.h"

#include "AthenaKernel/getMessageSvc.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "CLHEP/Matrix/Matrix.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/RtChebyshev.h"
#include "MdtCalibData/RtFromPoints.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtScaleFunction.h"
#include "MdtCalibFitters/CurvedPatRec.h"
#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
#include "MdtCalibRt/AdaptiveResidualSmoothing.h"
#include "MdtCalibRt/RtCalibrationOutput.h"
#include "MdtCalibRt/RtParabolicExtrapolation.h"
#include "MultilayerRtDifference.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibMath/BaseFunction.h"
#include "MuonCalibMath/BaseFunctionFitter.h"
#include "MuonCalibMath/ChebyshevPolynomial.h"
#include "MuonCalibMath/LegendrePolynomial.h"
#include "MuonCalibMath/PolygonBase.h"
#include "TF1.h"
#include "TProfile.h"
#include "sstream"

using namespace MuonCalib;

inline Double_t RtCalibrationCurved_polfun(Double_t *x, Double_t *par) { return par[0] * RtScalePolynomial(x[0]); }
RtCalibrationCurved::RtCalibrationCurved(const std::string &name) : IMdtCalibration(name), m_rt_accuracy_previous(0.0) {
    init(0.5 * CLHEP::mm, 1, 15, true, false, 15, false, false, false);
}

RtCalibrationCurved::RtCalibrationCurved(const std::string &name, const double &rt_accuracy, const unsigned int &func_type,
                                         const unsigned int &ord, const bool &fix_min, const bool &fix_max, const int &max_it,
                                         bool do_parabolic_extrapolation, bool do_smoothing, bool do_multilayer_rt_scale) :
    IMdtCalibration(name), m_rt_accuracy_previous(0.0) {
    init(rt_accuracy, func_type, ord, fix_min, fix_max, max_it, do_parabolic_extrapolation, do_smoothing, do_multilayer_rt_scale);
}

RtCalibrationCurved::~RtCalibrationCurved() {
    if (m_tfile) { m_tfile->Write(); }
}
double RtCalibrationCurved::reliability() const { return m_status; }

double RtCalibrationCurved::estimatedRtAccuracy() const { return m_rt_accuracy; }

int RtCalibrationCurved::numberOfSegments() const { return m_nb_segments; }

int RtCalibrationCurved::numberOfSegmentsUsed() const { return m_nb_segments_used; }

int RtCalibrationCurved::iteration() const { return m_iteration; }

bool RtCalibrationCurved::smoothing() const { return m_do_smoothing; }

void RtCalibrationCurved::setEstimateRtAccuracy(const double &acc) { m_rt_accuracy = std::abs(acc); }

void RtCalibrationCurved::switch_on_control_histograms(const std::string &file_name) {
    /////////////////////////////////////////////
    // CREATE THE ROOT FILE AND THE HISTOGRAMS //
    /////////////////////////////////////////////
    m_control_histograms = true;

    m_tfile = std::make_unique<TFile>(file_name.c_str(), "RECREATE");

    m_cut_evolution = std::make_unique<TH1F>("m_cut_evolution", "CUT EVOLUTION", 11, -0.5, 10.5);
    m_nb_segment_hits = std::make_unique<TH1F>("m_nb_segment_hits", "NUMBER OF HITS ON THE REFITTED SEGMENTS", 11, -0.5, 10.5);
    m_pull_initial = std::make_unique<TH1F>("m_pull_initial", "INITIAL PULL DISTRIBUTION", 200, -5.05, 5.05);
    m_residuals_initial =
        std::make_unique<TH2F>("m_residuals_initial", "INITIAL OF THE REFITTED SEGMENTS", 100, -0.5, 15.0, 300, -1.5, 1.5);
    m_residuals_initial_all = std::make_unique<TH2F>("m_residuals_initial_all", "INITIAL OF THE REFITTED SEGMENTS BEFORE CONVERGENCE", 300,
                                                     -15.0, 15.0, 1000, -5, 5);
    m_residuals_final = std::make_unique<TH2F>("m_residuals_final", "FINAL OF THE REFITTED SEGMENTS", 100, -0.5, 15.0, 300, -1.5, 1.5);
    m_driftTime_final =
        std::make_unique<TH2F>("m_driftTime_final", "FINAL DRIFTTIME OF THE REFITTED SEGMENTS", 300, -15.0, 15.0, 300, -15.0, 15.0);
    m_driftTime_initial =
        std::make_unique<TH2F>("m_driftTime_initial", "FINAL DRIFTTIME OF THE REFITTED SEGMENTS", 300, -15.0, 15.0, 1300, -100, 1200);
    m_adc_vs_residual_final =
        std::make_unique<TH2F>("m_adc_resi_initial", "FINAL ADC VS RESIDUAL OF THE REFITTED SEGMENTS", 1350, 0, 1350, 300, -15, 15);

    m_multilayer_rt_difference = std::make_unique<MultilayerRtDifference>(10000, m_tfile.get());
}
void RtCalibrationCurved::switch_off_control_histograms() {
    m_control_histograms = false;
    if (m_tfile) {
        m_tfile->Write();
        m_multilayer_rt_difference = std::make_unique<MultilayerRtDifference>(10000);
    }
}

void RtCalibrationCurved::forceMonotony() { m_force_monotony = true; }
void RtCalibrationCurved::doNotForceMonotony() { m_force_monotony = false; }
void RtCalibrationCurved::doParabolicExtrapolation() { m_do_parabolic_extrapolation = true; }
void RtCalibrationCurved::doSmoothing() { m_do_smoothing = true; }
void RtCalibrationCurved::noSmoothing() { m_do_smoothing = false; }
RtCalibrationCurved::MdtCalibOutputPtr RtCalibrationCurved::analyseSegments(const MuonSegVec &seg) {
    std::shared_ptr<const IRtRelation> tmp_rt;

    /////////////////////
    // Initial RESIDUALS //
    /////////////////////
    for (unsigned int k = 0; k < seg.size(); k++) {
        for (unsigned int l = 0; l < seg[k]->hitsOnTrack(); l++) {
            double rr = (seg[k]->mdtHOT())[l]->driftRadius();
            double dd = (seg[k]->mdtHOT())[l]->signedDistanceToTrack();
            if (m_residuals_final) m_residuals_initial_all->Fill(std::abs(dd), std::abs(rr) - std::abs(dd));
            m_driftTime_initial->Fill(rr, dd);
        }
    }

    //////////////////////////
    // AUTOCALIBRATION LOOP //
    //////////////////////////
    while (!converged()) {
        for (unsigned int k = 0; k < seg.size(); k++) { handleSegment(*seg[k]); }
        if (!analyse(seg)) {
            MsgStream log(Athena::getMessageSvc(), "RtCalibrationCurved");
            log << MSG::WARNING << "analyseSegments() - analyse failed, segments:" << endmsg;
            for (unsigned int i = 0; i < seg.size(); i++) {
                log << MSG::WARNING << i << " " << seg[i]->direction() << " " << seg[i]->position() << endmsg;
            }
            return nullptr;
        }

        const RtCalibrationOutput *rtOut = m_output.get();

        if (!converged()) { setInput(rtOut); }
        tmp_rt = rtOut->rt();
    }

    // parabolic extrapolations for small radii //
    if (m_do_parabolic_extrapolation) {
        std::shared_ptr<RtRelationLookUp> tmprt = performParabolicExtrapolation(true, true, *tmp_rt);
        m_output = std::make_shared<RtCalibrationOutput>(
            tmprt, std::make_shared<RtFullInfo>("RtCalibrationCurved", m_iteration, m_nb_segments_used, 0.0, 0.0, 0.0, 0.0));

        tmp_rt = tmprt;
    }

    //////////////////////////////////////////////
    // SMOOTHING AFTER CONVERGENCE IF REQUESTED //
    //////////////////////////////////////////////
    if (!m_do_smoothing) {
        // final residuals //
        double r{0}, d{0}, adc{0};
        for (unsigned int k = 0; k < seg.size(); k++) {
            for (unsigned int l = 0; l < seg[k]->hitsOnTrack(); l++) {
                adc = (seg[k]->mdtHOT())[l]->adcCount();
                r = (seg[k]->mdtHOT())[l]->driftRadius();
                d = (seg[k]->mdtHOT())[l]->signedDistanceToTrack();
                if (m_residuals_final) m_residuals_final->Fill(std::abs(d), std::abs(r) - std::abs(d));
                m_driftTime_final->Fill(r, d);
                m_adc_vs_residual_final->Fill(adc, std::abs(r) - std::abs(d));
            }
        }
        return getResults();
    }

    // maximum number of iterations //
    int max_smoothing_iterations(static_cast<int>(m_max_it));
    if (max_smoothing_iterations == 0) { max_smoothing_iterations = 1; }

    // convergence RMS //
    double convergence_RMS(0.002);

    // variables //
    int it(0);        // iteration
    double RMS(1.0);  // RMS change of r(t)

    // smoothing //
    //---------------------------------------------------------------------------//
    //---------------------------------------------------------------------------//
    while (it < max_smoothing_iterations && RMS > convergence_RMS) {
        //---------------------------------------------------------------------------//
        AdaptiveResidualSmoothing smoothing;

        // counter //
        unsigned int counter{0}, counter2{0};
        // overwrite drift radii and calculate the average resolution //
        for (unsigned int k = 0; k < seg.size(); k++) {
            if (seg[k]->mdtHitsOnTrack() < 4) { continue; }
            counter2++;
            double avres(0.0);
            for (unsigned int h = 0; h < seg[k]->mdtHitsOnTrack(); h++) {
                seg[k]->mdtHOT()[h]->setDriftRadius(tmp_rt->radius(seg[k]->mdtHOT()[h]->driftTime()),
                                                    seg[k]->mdtHOT()[h]->sigmaDriftRadius());
                if (seg[k]->mdtHOT()[h]->sigmaDriftRadius() < 0.5 * m_r_max) {
                    avres = avres + seg[k]->mdtHOT()[h]->sigma2DriftRadius();
                } else {
                    avres = avres + 0.01;
                }
            }
            avres = avres / static_cast<double>(seg[k]->mdtHitsOnTrack());
            avres = std::sqrt(avres);
            if (smoothing.addResidualsFromSegment(*seg[k], true, 5.0 * avres)) { counter++; }
        }

        // break, do no smoothing if there are not enough segments //
        if (counter < 1000) {
            MsgStream log(Athena::getMessageSvc(), "RtCalibrationCurved");
            log << MSG::WARNING << "analyseSegments() - too small number of reconstructed segments!" << endmsg;
            // final residuals //
            for (unsigned int k = 0; k < seg.size(); k++) {
                for (unsigned int l = 0; l < seg[k]->hitsOnTrack(); l++) {
                    double r = (seg[k]->mdtHOT())[l]->driftRadius();
                    double d = (seg[k]->mdtHOT())[l]->signedDistanceToTrack();
                    if (m_residuals_final) m_residuals_final->Fill(d, std::abs(r) - std::abs(d), 1.0);
                }
            }
            return getResults();
        }

        // smoothing //
        RtRelationLookUp smooth_rt(smoothing.performSmoothing(*tmp_rt, m_fix_min, m_fix_max));

        // calculate RMS change //
        RMS = 0.0;
        double bin_width(0.01 * (smooth_rt.tUpper() - smooth_rt.tLower()));
        for (double t = smooth_rt.tLower(); t <= smooth_rt.tUpper(); t = t + bin_width) {
            RMS = RMS + std::pow(smooth_rt.radius(t) - tmp_rt->radius(t), 2);
        }
        RMS = std::sqrt(0.01 * RMS);

        // increase the iterations counter //
        it++;

        // delete tmp_rt and update it //
        tmp_rt = std::make_shared<RtRelationLookUp>(smooth_rt);

        //---------------------------------------------------------------------------//
    }
    //---------------------------------------------------------------------------//
    //---------------------------------------------------------------------------//
    m_output = std::make_shared<RtCalibrationOutput>(
        tmp_rt, std::make_shared<RtFullInfo>("RtCalibrationCurved", m_iteration, m_nb_segments_used, 0.0, 0.0, 0.0, 0.0));

    /////////////////////
    // FINAL RESIDUALS //
    /////////////////////
    for (unsigned int k = 0; k < seg.size(); k++) {
        for (unsigned int l = 0; l < seg[k]->hitsOnTrack(); l++) {
            double adc = (seg[k]->mdtHOT())[l]->adcCount();
            double r = (seg[k]->mdtHOT())[l]->driftRadius();
            double d = (seg[k]->mdtHOT())[l]->signedDistanceToTrack();
            if (m_residuals_final) m_residuals_final->Fill(d, std::abs(r) - std::abs(d));
            m_driftTime_final->Fill(r, d);
            m_adc_vs_residual_final->Fill(adc, std::abs(r) - std::abs(d));
        }
    }

    /////////////////////////////////////////
    // RETURN THE RESULT AFTER CONVERGENCE //
    /////////////////////////////////////////
    return getResults();
}

//*****************************************************************************

//::::::::::::::::::::::::::
//:: METHOD handleSegment ::
//::::::::::::::::::::::::::

bool RtCalibrationCurved::handleSegment(MuonCalibSegment &seg) {
    /////////////////////////////////////////////////
    // RETURN, IF THE SEGMENT HAD LESS THAN 4 HITS //
    /////////////////////////////////////////////////
    if (m_control_histograms) { m_cut_evolution->Fill(0.0, 1.0); }

    m_nb_segments = m_nb_segments + 1;
    if (seg.mdtHitsOnTrack() < 4) { return true; }

    if (m_control_histograms) { m_cut_evolution->Fill(1.0, 1.0); }

    if (std::isnan(seg.direction().x()) || std::isnan(seg.direction().y()) || std::isnan(seg.direction().z()) ||
        std::isnan(seg.position().x()) || std::isnan(seg.position().y()) || std::isnan(seg.position().z())) {
        return true;
    }
    if (std::abs(seg.direction().y()) > 100) { return true; }

    ///////////////
    // VARIABLES //
    ///////////////

    // segment reconstruction and segment selection //
    double aux_res;            // auxiliary resolution
    double av_res(0.0);        // average spatial resolution of the tube hits
    double chi2_scale_factor;  // chi^2 scale factor used to take into
    // account bad r-t accuracy in the segment
    // selection
    IMdtSegmentFitter::HitSelection hit_selection(seg.mdtHitsOnTrack());
    // hit selection vectors for refits
    unsigned int nb_hits_in_ml;   // number of hits in the multilayers
    double x;                     // reduced time = (r(t)-0.5*m_r_max)/(0.5*m_r_max)
    std::vector<double> d_track;  // signed distances of the track from the anode
    // wires of the tubes
    std::vector<MTStraightLine> w;      // anode wires
    Amg::Vector3D null(0.0, 0.0, 0.0);  // auxiliary 0 vector
    Amg::Vector3D xhat(1.0, 0.0, 0.0);  // auxiliary unit vector

    // objects needed to calculate the autocalibration matrix and vector //
    std::vector<CLHEP::HepVector> F;  // auxiliary vectors for the calculation of the
    // track cooeffients matrix
    CLHEP::HepVector residual_value;     // residuals values
    CLHEP::HepVector weighted_residual;  // residual values weighted by the inverse
    // variance of the radius measurements
    CLHEP::HepMatrix D;  // Jacobian of the residuals (dresiduals/dr)
                         //     static ofstream display("display.kumac");

    ///////////////////////////////////////
    // PREPARATION FOR THE SEGMENT REFIT //
    ///////////////////////////////////////

    // overwrite the drift radii according to the input r-t relationship, //
    // calculate the average spatial resolution to define a road width,   //
    // select the hits in the chamber                                     //
    nb_hits_in_ml = 0;
    for (unsigned int k = 0; k < seg.mdtHitsOnTrack(); k++) {
        // make the resolution at small radii large enough //
        aux_res = seg.mdtHOT()[k]->sigmaDriftRadius();
        if (m_rt->radius(seg.mdtHOT()[k]->driftTime()) < 0.75) {
            if (aux_res < 0.5 - m_rt->radius(seg.mdtHOT()[k]->driftTime())) { aux_res = 0.5 - m_rt->radius(seg.mdtHOT()[k]->driftTime()); }
        }

        // overwrite radius //
        seg.mdtHOT()[k]->setDriftRadius(m_rt->radius(seg.mdtHOT()[k]->driftTime()), aux_res);

        // hit selection //
        hit_selection[k] = 0;

        // reject hits with bad radii //
        if (seg.mdtHOT()[k]->driftRadius() < 0.0 || seg.mdtHOT()[k]->driftRadius() > m_r_max ||
            seg.mdtHOT()[k]->sigmaDriftRadius() > 10.0 * m_r_max) {
            hit_selection[k] = 1;
        }

        // average resolution in the segment //
        if (hit_selection[k] == 0) {
            if (seg.mdtHOT()[k]->sigmaDriftRadius() < 0.5 * m_r_max) {
                av_res = av_res + std::pow(seg.mdtHOT()[k]->sigmaDriftRadius(), 2);
            } else {
                av_res = av_res + 0.01;
            }
        }

        // counting of selected segments //
        nb_hits_in_ml = nb_hits_in_ml + (1 - hit_selection[k]);
    }

    // average resolution and chi^2 scale factor //
    av_res = std::sqrt(av_res / static_cast<double>(seg.mdtHitsOnTrack()));
    chi2_scale_factor = std::hypot(av_res, m_rt_accuracy) / av_res;

    ///////////////////////////////////////
    // FILL THE AUTOCALIBRATION MATRICES //
    ///////////////////////////////////////

    // set the road width for the track reconstruction //
    m_tracker->setRoadWidth(7.0 * std::hypot(av_res, m_rt_accuracy));

    // check whether there are enough hits in the chambers //
    if (nb_hits_in_ml < 4) { return true; }

    // refit the segments //
    CurvedLine track;

    if (!m_tracker->fit(seg, hit_selection, track)) { return true; }

    if (std::isnan(track.chi2())) { return true; }

    // check the quality of the fit //
    if (track.chi2PerDegreesOfFreedom() > 5 * chi2_scale_factor) { return true; }

    // check whether there are at least three track hits //
    if (m_control_histograms) { m_nb_segment_hits->Fill(track.numberOfTrackHits()); }
    if (track.numberOfTrackHits() < 4) { return true; }

    // 	display_segment(&seg, display&(m_tracker->curvedTrack()));

    // reject tracks with silly parameters
    if (std::abs(track.getTangent(seg.mdtHOT()[0]->localPosition().z()).a_x2()) > 8.0e8) { return true; }

    // bookkeeping for convergence decision and reliability estimation //
    m_chi2 += track.chi2PerDegreesOfFreedom();
    m_nb_segments_used = m_nb_segments_used + 1;

    // fill the autocalibration objects //

    // track coeffient matrix and its inverse //
    F = std::vector<CLHEP::HepVector>(track.numberOfTrackHits());
    for (unsigned int h = 0; h < track.numberOfTrackHits(); h++) {
        const MdtCalibHitBase &hb = *(track.trackHits()[h]);
        m_multilayer_rt_difference->Fill(hb, *m_rt);

        F[h] = CLHEP::HepVector(m_M_track.num_row());
        for (int p = 0; p < F[h].num_row(); p++) {
            double x = std::sqrt(1.0 + std::pow(track.getTangent((track.trackHits()[h]->localPosition()).z()).a_x2(), 2));
            if (x) {
                (F[h])[p] = m_Legendre->value(p, (track.trackHits()[h]->localPosition()).z()) / x;
            } else {
                (F[h])[p] = 0.;
            }
        }
    }
    for (int p = 0; p < m_M_track.num_row(); p++) {
        for (int q = p; q < m_M_track.num_row(); q++) {
            m_M_track[p][q] = 0.0;
            for (unsigned int h = 0; h < track.numberOfTrackHits(); h++) {
                m_M_track[p][q] = m_M_track[p][q] + (F[h])[p] * (F[h])[q] / (track.trackHits()[h])->sigma2DriftRadius();
            }
            m_M_track[q][p] = m_M_track[p][q];
        }
    }
    int ifail;
    m_M_track_inverse = m_M_track.inverse(ifail);
    if (ifail != 0) {
        MsgStream log(Athena::getMessageSvc(), "RtCalibrationCurved");
        log << MSG::WARNING << "handleSegment() - Could not invert track matrix!" << endmsg;
        return true;
    }

    // Jacobian matrix for the residuals //
    // track distances, residuals, corrections //
    d_track = std::vector<double>(track.numberOfTrackHits());
    w = std::vector<MTStraightLine>(track.numberOfTrackHits());
    residual_value = CLHEP::HepVector(track.numberOfTrackHits());
    weighted_residual = CLHEP::HepVector(track.numberOfTrackHits());
    for (unsigned int l = 0; l < m_order; l++) {
        m_U[l] = CLHEP::HepVector(track.numberOfTrackHits());
        m_U_weighted[l] = CLHEP::HepVector(track.numberOfTrackHits());
    }
    for (unsigned int h = 0; h < track.numberOfTrackHits(); h++) {
        w[h] = MTStraightLine(Amg::Vector3D(0.0, (track.trackHits()[h]->localPosition()).y(), (track.trackHits()[h]->localPosition()).z()),
                              xhat, null, null);
        d_track[h] = track.getTangent((track.trackHits()[h]->localPosition()).z()).signDistFrom(w[h]);
        residual_value[h] = (track.trackHits()[h])->driftRadius() - std::abs(d_track[h]);
        if (m_control_histograms) {
            if (m_iteration == 0) { m_residuals_initial->Fill(std::abs(d_track[h]), residual_value[h], 1.0); }
        }
        for (unsigned int l = 0; l < m_order; l++) {
            x = (track.trackHits()[h]->driftRadius() - 0.5 * m_r_max) / (0.5 * m_r_max);
            (m_U[l])[h] = m_base_function->value(l, x);
        }
    }

    // Jacobian //
    D = CLHEP::HepMatrix(track.numberOfTrackHits(), track.numberOfTrackHits());
    for (unsigned int h = 0; h < track.numberOfTrackHits(); h++) {
        for (unsigned int hp = 0; hp < track.numberOfTrackHits(); hp++) {
            D[h][hp] = (h == hp) - (2 * (d_track[h] >= 0) - 1) * (2 * (d_track[hp] >= 0) - 1) * dot(F[h], m_M_track_inverse * F[hp]) /
                                       (track.trackHits()[hp])->sigma2DriftRadius();
        }
    }

    // autocalibration objects //
    // errors of the residuals //
    std::vector<double> sigma_residual(track.numberOfTrackHits(), 0.0);
    for (unsigned int h = 0; h < track.numberOfTrackHits(); h++) {
        for (unsigned int hp = 0; hp < track.numberOfTrackHits(); hp++) {
            sigma_residual[h] = sigma_residual[h] + std::pow(D[h][hp] * (track.trackHits()[hp])->sigmaDriftRadius(), 2);
        }
        sigma_residual[h] = std::sqrt(sigma_residual[h]);
        if (sigma_residual[h] < av_res / track.numberOfTrackHits()) { sigma_residual[h] = av_res / std::sqrt(track.numberOfTrackHits()); }
    }

    // weight residuals and correction functions //
    for (unsigned int h = 0; h < track.numberOfTrackHits(); h++) {
        weighted_residual[h] = residual_value[h] / sigma_residual[h];
        if (m_control_histograms) {
            if (m_iteration == 0) { m_pull_initial->Fill(weighted_residual[h], 1.0); }
        }
        for (unsigned int l = 0; l < m_order; l++) { (m_U_weighted[l])[h] = (m_U[l])[h] / sigma_residual[h]; }
    }

    // autocalibration matrix and autocalibration vector //
    CLHEP::HepSymMatrix A_tmp(m_A);

    // autocalibration objects //
    for (unsigned int p = 0; p < m_order; p++) {
        for (unsigned int pp = p; pp < m_order; pp++) {
            A_tmp[p][pp] = m_A[p][pp] + dot(D * m_U_weighted[p], D * m_U_weighted[pp]);
            if (std::isnan(A_tmp[p][pp])) { return true; }
        }
    }

    CLHEP::HepVector b_tmp(m_b);
    for (unsigned int p = 0; p < m_order; p++) {
        b_tmp[p] = m_b[p] + dot(D * m_U_weighted[p], weighted_residual);
        if (std::isnan(b_tmp[p])) { return true; }
    }

    m_A = A_tmp;
    m_b = b_tmp;

    return true;
}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD setInput ::
//:::::::::::::::::::::
void RtCalibrationCurved::setInput(const IMdtCalibrationOutput *rt_input) {
    ///////////////
    // VARIABLES //
    ///////////////
    const RtCalibrationOutput *input(dynamic_cast<const RtCalibrationOutput *>(rt_input));

    ////////////////////////////////////////////
    // CHECK IF THE OUTPUT CLASS IS SUPPORTED //
    ////////////////////////////////////////////
    if (input == nullptr) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRtCalibrationCurved::setInput - Calibration input class not supported.", __FILE__, __LINE__));
    }

    /////////////////////////////////////////////////////////////////
    // GET THE INITIAL r-t RELATIONSHIP AND RESET STATUS VARIABLES //
    /////////////////////////////////////////////////////////////////

    // get the r-t relationship //
    m_rt = input->rt();

    // status variables //
    m_nb_segments = 0;
    m_nb_segments_used = 0;
    m_chi2 = 0.0;
    m_A = CLHEP::HepSymMatrix(m_order, 0);
    m_b = CLHEP::HepVector(m_order, 0);
    m_alpha = CLHEP::HepVector(m_order, 0);

    // drift-time interval //
    std::shared_ptr<const RtChebyshev> rt_Chebyshev = std::dynamic_pointer_cast<const RtChebyshev>(m_rt);
    std::shared_ptr<const RtRelationLookUp> rt_LookUp = std::dynamic_pointer_cast<const RtRelationLookUp>(m_rt);

    if (!rt_Chebyshev && !rt_LookUp) {
        throw std::runtime_error(Form("File: %s, Line: %d\nRtCalibrationCurved::setInput - r-t class not supported.", __FILE__, __LINE__));
    }

    // RtChebyshev //
    if (rt_Chebyshev) {
        m_t_length = rt_Chebyshev->tUpper() - rt_Chebyshev->tLower();
        m_t_mean = 0.5 * (rt_Chebyshev->tLower() + rt_Chebyshev->tUpper());
    }

    // RtRelationLookUp, dangerous implementation, but the only way right now //
    if (rt_LookUp) {
        m_t_length = rt_LookUp->par(1) * (rt_LookUp->nPar() - 2) - rt_LookUp->par(0);
        m_t_mean = 0.5 * (rt_LookUp->par(1) * (rt_LookUp->nPar() - 2) + rt_LookUp->par(0));
    }

    return;
}

//*****************************************************************************

//::::::::::::::::::::
//:: METHOD analyse ::
//::::::::::::::::::::
bool RtCalibrationCurved::analyse(const MuonSegVec &seg) {
    ///////////////
    // VARIABLES //
    ///////////////
    int ifail;                   // flag indicating a failure of the matrix inversion
    unsigned int nb_points(30);  // number of points used to set the new r-t relationship
    double step;                 // r step size
    std::shared_ptr<const RtChebyshev> rt_Chebyshev = std::dynamic_pointer_cast<const RtChebyshev>(m_rt);
    std::shared_ptr<const RtRelationLookUp> rt_LookUp = std::dynamic_pointer_cast<const RtRelationLookUp>(m_rt);
    double r_corr;                               // radial correction
    std::vector<double> rt_param(m_rt->nPar());  // parameters for the new r-t
    double x;                                    // reduced time
    RtFromPoints rt_from_points;                 // r-t from points

    ////////////////////////////////////////
    // SOLVE THE AUTOCALIBRATION EQUATION //
    ////////////////////////////////////////

    m_alpha = m_A.inverse(ifail) * m_b;
    if (ifail != 0) {
        MsgStream log(Athena::getMessageSvc(), "RtCalibrationCurved");
        log << MSG::WARNING << "analyse() - Could not solve the autocalibration equation!" << endmsg;
        return false;
    }

    ////////////////////////////////////////
    // CALCULATE THE NEW r-t RELATIONSHIP //
    ////////////////////////////////////////

    // input r-t is of type RtChebyshev //
    if (rt_Chebyshev) {
        // set the number of points //
        if (rt_Chebyshev->numberOfRtParameters() > 30) { nb_points = rt_Chebyshev->numberOfRtParameters(); }

        // r step size //
        step = m_r_max / static_cast<double>(nb_points);

        // sample points and Chebyshev fitter //
        std::vector<SamplePoint> x_r(nb_points + 1);
        BaseFunctionFitter fitter(rt_Chebyshev->numberOfRtParameters());
        ChebyshevPolynomial chebyshev;

        // calculate the sample points //
        for (unsigned int k = 0; k < nb_points + 1; k++) {
            x_r[k].set_x1(t_from_r(k * step));
            x_r[k].set_x2(rt_Chebyshev->radius(x_r[k].x1()));
            x_r[k].set_x1(rt_Chebyshev->get_reduced_time(x_r[k].x1()));
            x_r[k].set_error(1.0);

            r_corr = 0.0;
            for (unsigned int l = 0; l < m_order; l++) {
                // 				r_corr = r_corr+m_alpha[l]*
                // 					m_base_function->value(l, x_r[k].x1());
                r_corr = r_corr + m_alpha[l] * m_base_function->value(l, (x_r[k].x2() - 0.5 * m_r_max) / (0.5 * m_r_max));
            }

            // do not change the r-t and the endpoints //
            if (((k == 0 || x_r[k].x2() < 0.5) && m_fix_min) || ((k == nb_points || x_r[k].x2() > m_r_max) && m_fix_max)) {
                r_corr = 0.0;
                x_r[k].set_error(0.01);
            }
            x_r[k].set_x2(x_r[k].x2() - r_corr);
        }

        // force monotony //
        if (m_force_monotony) {
            for (unsigned int k = 0; k < nb_points; k++) {
                if (x_r[k].x2() > x_r[k + 1].x2()) { x_r[k + 1].set_x2(x_r[k].x2()); }
            }
        }

        // create the new r-t relationship //
        fitter.fit_parameters(x_r, 1, nb_points + 1, &chebyshev);
        rt_param[0] = rt_Chebyshev->tLower();
        rt_param[1] = rt_Chebyshev->tUpper();
        for (unsigned int k = 0; k < rt_Chebyshev->numberOfRtParameters(); k++) { rt_param[k + 2] = fitter.coefficients()[k]; }

        m_rt_new = std::make_shared<RtChebyshev>(rt_param);
        m_rt_new->SetTmaxDiff(m_rt->GetTmaxDiff());
    }

    // input-rt is of type RtRelationLookUp //
    if (rt_LookUp) {
        rt_param = rt_LookUp->parameters();
        unsigned int min_k(2), max_k(rt_param.size());
        if (m_fix_min) { min_k = 3; }
        if (m_fix_max) { max_k = rt_param.size() - 1; }
        for (unsigned int k = min_k; k < max_k; k++) {
            x = (rt_param[k] - 0.5 * m_r_max) / (0.5 * m_r_max);
            r_corr = m_alpha[0];
            for (unsigned int l = 1; l < m_order; l++) { r_corr = r_corr + m_alpha[l] * m_base_function->value(l, x); }
            rt_param[k] = rt_param[k] - r_corr;
            if (m_force_monotony && k > 2) {
                if (rt_param[k] < rt_param[k - 1]) { rt_param[k] = rt_param[k - 1]; }
            }
        }

        m_rt_new = std::make_shared<RtRelationLookUp>(rt_param);
        m_rt_new->SetTmaxDiff(m_rt->GetTmaxDiff());
    }

    /////////////////////////////////////////////////////////
    // DETERMINE THE r-t QUALITY AND CHECK FOR CONVERGENCE //
    /////////////////////////////////////////////////////////

    // estimate r-t accuracy //
    m_rt_accuracy = 0.0;
    //	double m_rt_accuracy_diff = 0.0;
    double r_corr_max = 0.0;

    for (unsigned int k = 0; k < 100; k++) {
        r_corr = m_alpha[0];
        for (unsigned int l = 1; l < m_order; l++) { r_corr = r_corr + m_alpha[l] * m_base_function->value(l, -1.0 + 0.01 * k); }
        if (std::abs(r_corr) > r_corr_max) { r_corr_max = std::abs(r_corr); }
        m_rt_accuracy = m_rt_accuracy + r_corr * r_corr;
    }
    m_rt_accuracy = std::sqrt(0.01 * m_rt_accuracy);
    //	m_rt_accuracy_diff = m_rt_accuracy_previous - m_rt_accuracy;
    m_rt_accuracy_previous = m_rt_accuracy;

    // convergence? //
    m_chi2 = m_chi2 / static_cast<double>(m_nb_segments_used);
    if ((m_chi2 <= m_chi2_previous || std::abs(m_chi2 - m_chi2_previous) > 0.01) ||
        (std::abs(m_rt_accuracy) > 0.001 && m_iteration < m_max_it)) {
        m_status = 0;  // no convergence yet
    } else {
        m_status = 1;
    }
    m_chi2_previous = m_chi2;

    // reliabilty of convergence //
    if (m_status != 0) {
        if (m_nb_segments_used < 0.5 * m_nb_segments) { m_status = 2; }
    }

    //////////////////////////////////////
    // Set new multilayer rt-difference //
    //////////////////////////////////////
    if (m_iteration > 0 && m_do_multilayer_rt_scale) {
        m_multilayer_rt_difference->DoFit(m_rt_new.get(), seg);
    } else {
        m_multilayer_rt_difference->DoFit(nullptr, {});
    }

    //////////////////////////////////////////////////
    // STORE THE RESULTS IN THE RtCalibrationOutput //
    //////////////////////////////////////////////////

    m_iteration = m_iteration + 1;

    m_output = std::make_shared<RtCalibrationOutput>(
        m_rt_new, std::make_shared<RtFullInfo>("RtCalibrationCurved", m_iteration, m_nb_segments_used, 0.0, 0.0, 0.0, 0.0));

    return true;
}
bool RtCalibrationCurved::converged() const { return (m_status > 0); }
RtCalibrationCurved::MdtCalibOutputPtr RtCalibrationCurved::getResults() const { return m_output; }

//*****************************************************************************

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::
void RtCalibrationCurved::init(const double &rt_accuracy, const unsigned int &func_type, const unsigned int &ord, const bool &fix_min,
                               const bool &fix_max, const int &max_it, bool do_parabolic_extrapolation, bool do_smoothing,
                               bool do_multilayer_rt_scale) {
    /////////////////////////////
    // RESET PRIVATE VARIABLES //
    /////////////////////////////
    m_rt = nullptr;
    m_r_max = 15.0 * CLHEP::mm;
    m_control_histograms = false;
    m_nb_segments = 0;
    m_nb_segments_used = 0;
    m_iteration = 0;
    m_multilayer[0] = false;
    m_multilayer[1] = false;
    m_status = 0;
    m_rt_accuracy = std::abs(rt_accuracy);
    m_chi2_previous = 1.0e99;  // large value to force at least two rounds
    m_chi2 = 0.0;
    m_order = ord;
    m_fix_min = fix_min;
    m_fix_max = fix_max;
    m_max_it = std::abs(max_it);
    m_do_multilayer_rt_scale = do_multilayer_rt_scale;
    m_multilayer_rt_difference = std::make_unique<MultilayerRtDifference>(10000);

    m_tracker = std::make_unique<CurvedPatRec>();

    if (m_order == 0) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRtCalibrationCurved::init - Order of the correction polynomial must be >0!", __FILE__, __LINE__));
    }

    m_t_length = 1000.0;
    m_t_mean = 500.0;
    // default values, correct values will be set when the input r-t
    // has been given

    m_U = std::vector<CLHEP::HepVector>(m_order);
    m_U_weighted = std::vector<CLHEP::HepVector>(m_order);
    m_A = CLHEP::HepSymMatrix(m_order, 0);
    m_b = CLHEP::HepVector(m_order, 0);
    m_alpha = CLHEP::HepVector(m_order, 0);

    // correction function
    if (func_type < 1 || func_type > 3) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRtCalibrationCurved::init - Illegal correction function type!", __FILE__, __LINE__));
    }
    switch (func_type) {
        case 1: m_base_function = std::make_unique<LegendrePolynomial>(); break;
        case 2: m_base_function = std::make_unique<ChebyshevPolynomial>(); break;
        case 3:
            if (m_order < 2) {
                throw std::runtime_error(
                    Form("File: %s, Line: %d\nRtCalibrationCurved::init - Order must be >2 for polygons! It is set to %i by the user.",
                         __FILE__, __LINE__, m_order));
            }
            std::vector<double> x(m_order);
            double bin_width = 2.0 / static_cast<double>(m_order - 1);
            for (unsigned int k = 0; k < m_order; k++) { x[k] = -1 + k * bin_width; }
            m_base_function = std::make_unique<PolygonBase>(x);
            break;
    }

    // monotony of r(t) //
    m_force_monotony = false;

    // parabolic extrapolations //
    m_do_parabolic_extrapolation = do_parabolic_extrapolation;

    // smoothing //
    m_do_smoothing = do_smoothing;

    // Legendre polynomials and tracking objects //
    m_Legendre = Legendre_polynomial::get_Legendre_polynomial();
    m_M_track = CLHEP::HepSymMatrix(3);
    m_M_track_inverse = CLHEP::HepSymMatrix(3);

    return;
}
double RtCalibrationCurved::t_from_r(const double &r) {
    ///////////////
    // VARIABLES //
    ///////////////
    double precision(0.001);                            // spatial precision of the inversion
    double t_max(0.5 * (m_t_length + 2.0 * m_t_mean));  // upper time search limit
    double t_min(t_max - m_t_length);                   // lower time search limit

    /////////////////////////////////////////////
    // SEARCH FOR THE CORRESPONDING DRIFT TIME //
    /////////////////////////////////////////////
    while (t_max - t_min > 0.1 && std::abs(m_rt->radius(0.5 * (t_min + t_max)) - r) > precision) {
        if (m_rt->radius(0.5 * (t_min + t_max)) > r) {
            t_max = 0.5 * (t_min + t_max);
        } else {
            t_min = 0.5 * (t_min + t_max);
        }
    }

    return 0.5 * (t_min + t_max);
}
void RtCalibrationCurved::display_segment(MuonCalibSegment *segment, std::ofstream &outfile, const CurvedLine *curved_segment) {
    ///////////////
    // VARIABLES //
    ///////////////
    double y_min, y_max, z_min, z_max;  // minimum and maximum y and z coordinates
    Amg::Vector3D null(0.0, 0.0, 0.0);  // auxiliary null vector

    /////////////////////////
    // DISPLAY THE SEGMENT //
    /////////////////////////
    // minimum and maximum coordinates //
    y_min = (segment->mdtHOT()[0])->localPosition().y();
    y_max = y_min;
    z_min = (segment->mdtHOT()[0])->localPosition().z();
    z_max = z_min;
    for (unsigned int k = 1; k < segment->mdtHitsOnTrack(); k++) {
        if ((segment->mdtHOT()[k])->localPosition().y() < y_min) { y_min = (segment->mdtHOT()[k])->localPosition().y(); }
        if ((segment->mdtHOT()[k])->localPosition().y() > y_max) { y_max = (segment->mdtHOT()[k])->localPosition().y(); }
        if ((segment->mdtHOT()[k])->localPosition().z() < z_min) { z_min = (segment->mdtHOT()[k])->localPosition().z(); }
        if ((segment->mdtHOT()[k])->localPosition().z() > z_max) { z_max = (segment->mdtHOT()[k])->localPosition().z(); }
    }
    for (unsigned int k = 0; k < segment->mdtCloseHits(); k++) {
        if ((segment->mdtClose()[k])->localPosition().y() < y_min) { y_min = (segment->mdtClose()[k])->localPosition().y(); }
        if ((segment->mdtClose()[k])->localPosition().y() > y_max) { y_max = (segment->mdtClose()[k])->localPosition().y(); }
        if ((segment->mdtClose()[k])->localPosition().z() < z_min) { z_min = (segment->mdtClose()[k])->localPosition().z(); }
        if ((segment->mdtClose()[k])->localPosition().z() > z_max) { z_max = (segment->mdtClose()[k])->localPosition().z(); }
    }

    // write out the coordinate system //
    if (y_max - y_min > z_max - z_min) {
        outfile << "nullptr " << y_min - 30.0 << " " << y_max + 30.0 << " " << 0.5 * (z_min + z_max) - 0.5 * (y_max - y_min) - 30.0 << " "
                << 0.5 * (z_min + z_max) + 0.5 * (y_max - y_min) + 30.0 << "\n";
    } else {
        outfile << "nullptr " << 0.5 * (y_min + y_max) - 0.5 * (z_max - z_min) - 30.0 << " "
                << 0.5 * (y_min + y_max) + 0.5 * (z_max - z_min) + 30.0 << " " << z_min - 30.0 << " " << z_max + 30.0 << "\n";
    }

    // write out the hits on track //
    for (unsigned int k = 0; k < segment->mdtHitsOnTrack(); k++) {
        // draw a circle for the tube //
        outfile << "SET PLCI 1\n"
                << "ARC " << (segment->mdtHOT()[k])->localPosition().y() << " " << (segment->mdtHOT()[k])->localPosition().z() << " 15.0\n";

        // draw a drift circle //
        outfile << "SET PLCI 3\n"
                << "ARC " << (segment->mdtHOT()[k])->localPosition().y() << " " << (segment->mdtHOT()[k])->localPosition().z() << " "
                << (segment->mdtHOT()[k])->driftRadius() << "\n";
    }

    // write out the close hits //
    for (unsigned int k = 0; k < segment->mdtCloseHits(); k++) {
        // draw a circle for the tube //
        outfile << "SET PLCI 1\n"
                << "ARC " << (segment->mdtClose()[k])->localPosition().y() << " " << (segment->mdtClose()[k])->localPosition().z()
                << " 15.0\n";

        // draw a drift circle //
        outfile << "SET PLCI 2\n"
                << "ARC " << (segment->mdtClose()[k])->localPosition().y() << " " << (segment->mdtClose()[k])->localPosition().z() << " "
                << (segment->mdtClose()[k])->driftRadius() << "\n";
    }

    // write out the reconstructed track //
    // a straight line is drawn by default //
    if (curved_segment == nullptr) {
        MTStraightLine aux_track(segment->position(), segment->direction(), null, null);
        outfile << "SET PLCI 4\n"
                << "LINE " << aux_track.a_x2() * (z_min - 30.0) + aux_track.b_x2() << " " << z_min - 30.0 << " "
                << aux_track.a_x2() * (z_max + 30.0) + aux_track.b_x2() << " " << z_max + 30.0 << "\n";
    }

    // a curved segment is drawn on demand //
    if (curved_segment != nullptr) {
        double step_size((60.0 + z_max - z_min) / 50.0);
        for (double aux_z = z_min; aux_z <= z_max; aux_z = aux_z + step_size) {
            outfile << "SET PLCI 4\n"
                    << "LINE " << curved_segment->getPointOnLine(aux_z).y() << " " << aux_z << " "
                    << curved_segment->getPointOnLine(aux_z + step_size).y() << " " << aux_z + step_size << "\n";
        }
    }

    // add a wait statement //
    outfile << "WAIT\n";

    return;
}
std::shared_ptr<RtRelationLookUp> RtCalibrationCurved::performParabolicExtrapolation(const bool &min, const bool &max,
                                                                                     const IRtRelation &in_rt) {
    ///////////////
    // VARIABLES //
    ///////////////
    RtParabolicExtrapolation rt_extrapolator;           // r-t extrapolator
    std::shared_ptr<RtRelationLookUp> rt_low, rt_high;  // pointers to the r-t relationships after extrapolation
    std::vector<SamplePoint> add_fit_point;             // additional r-t points used if r(0) or r(t_max) is fixed.

    ////////////////////////////////
    // EXTRAPOLATE TO LARGE RADII //
    ////////////////////////////////
    if (max) {
        add_fit_point.clear();
        if (m_fix_max) { add_fit_point.push_back(SamplePoint(in_rt.tUpper(), in_rt.radius(in_rt.tUpper()), 1.0)); }
        if (in_rt.radius(in_rt.tUpper()) < 16.0) {
            rt_high = std::make_shared<RtRelationLookUp>(rt_extrapolator.getRtWithParabolicExtrapolation(
                in_rt, in_rt.radius(in_rt.tUpper()) - 3.0, in_rt.radius(in_rt.tUpper()) - 1.0, in_rt.radius(in_rt.tUpper()),
                add_fit_point));
        } else {
            rt_high =
                std::make_shared<RtRelationLookUp>(rt_extrapolator.getRtWithParabolicExtrapolation(in_rt, 13., 15., 16., add_fit_point));
        }
    }

    ////////////////////////////////
    // EXTRAPOLATE TO SMALL RADII //
    ////////////////////////////////
    if (min) {
        add_fit_point.clear();
        if (m_fix_min) { add_fit_point.push_back(SamplePoint(m_rt_new->tLower(), 0.0, 1.0)); }
        if (m_fix_max && rt_high != nullptr) {
            rt_low =
                std::make_shared<RtRelationLookUp>(rt_extrapolator.getRtWithParabolicExtrapolation(*rt_high, 1.0, 3.0, 0.0, add_fit_point));
        } else {
            rt_low =
                std::make_shared<RtRelationLookUp>(rt_extrapolator.getRtWithParabolicExtrapolation(in_rt, 1.0, 3.0, 0.0, add_fit_point));
        }
    }

    ////////////////////
    // RETURN RESULTS //
    ////////////////////
    if (min && max) {
        if (in_rt.HasTmaxDiff()) { rt_low->SetTmaxDiff(in_rt.GetTmaxDiff()); }
        return rt_low;
    }
    if (min) {
        if (in_rt.HasTmaxDiff()) { rt_low->SetTmaxDiff(in_rt.GetTmaxDiff()); }
        return rt_low;
    }
    if (in_rt.HasTmaxDiff() && rt_high) { rt_high->SetTmaxDiff(in_rt.GetTmaxDiff()); }
    return rt_high;
}
