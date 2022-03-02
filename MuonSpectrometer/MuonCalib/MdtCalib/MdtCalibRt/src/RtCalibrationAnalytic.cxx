/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibRt/RtCalibrationAnalytic.h"

#include <TString.h>  // for Form

#include "AthenaKernel/getMessageSvc.h"
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/RtChebyshev.h"
#include "MdtCalibData/RtFromPoints.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
#include "MdtCalibRt/AdaptiveResidualSmoothing.h"
#include "MdtCalibRt/RtCalibrationOutput.h"
#include "MdtCalibRt/RtParabolicExtrapolation.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibMath/BaseFunction.h"
#include "MuonCalibMath/BaseFunctionFitter.h"
#include "MuonCalibMath/ChebyshevPolynomial.h"
#include "MuonCalibMath/LegendrePolynomial.h"
#include "MuonCalibMath/PolygonBase.h"
#include "TGraphErrors.h"
#include "cmath"
#include "sstream"

using namespace MuonCalib;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//:: IMPLEMENTATION OF METHODS DEFINED IN THE CLASS RtCalibrationAnalytic ::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//*****************************************************************************

RtCalibrationAnalytic::RtCalibrationAnalytic(const std::string &name) : IMdtCalibration(name) {
    init(0.5 * CLHEP::mm, 1, 5, true, true, true, true, 100, false, false);
}

RtCalibrationAnalytic::RtCalibrationAnalytic(const std::string &name, const double &rt_accuracy, const unsigned int &func_type,
                                             const unsigned int &ord, const bool &split, const bool &full_matrix, const bool &fix_min,
                                             const bool &fix_max, const int &max_it, bool do_smoothing, bool do_parabolic_extrapolation) :
    IMdtCalibration(name), m_rt(nullptr) {
    init(rt_accuracy, func_type, ord, split, full_matrix, fix_min, fix_max, max_it, do_smoothing, do_parabolic_extrapolation);
}

RtCalibrationAnalytic::~RtCalibrationAnalytic() {
    if (m_tfile) { m_tfile->Write(); }
}

//:::::::::::::::::
//:: METHOD init ::
//:::::::::::::::::
void RtCalibrationAnalytic::init(const double &rt_accuracy, const unsigned int &func_type, const unsigned int &ord, const bool &split,
                                 const bool &full_matrix, const bool &fix_min, const bool &fix_max, const int &max_it, bool do_smoothing,
                                 bool do_parabolic_extrapolation) {
    /////////////////////////////
    // RESET PRIVATE VARIABLES //
    /////////////////////////////
    m_r_max = 15.0 * CLHEP::mm;
    m_control_histograms = false;
    m_tfile = nullptr;
    m_cut_evolution = nullptr;
    m_nb_segment_hits = nullptr;
    m_CL = nullptr;
    m_residuals = nullptr;
    m_split_into_ml = split;
    m_full_matrix = full_matrix;
    m_nb_segments = 0;
    m_nb_segments_used = 0;
    m_iteration = 0;
    m_multilayer[0] = false;
    m_multilayer[1] = false;
    m_status = 0;
    m_rt_accuracy = std::abs(rt_accuracy);
    m_rt_accuracy_previous = 0.;
    m_chi2_previous = 1.0e99;  // large value to force at least two rounds
    m_chi2 = 0.0;
    m_order = ord;
    m_fix_min = fix_min;
    m_fix_max = fix_max;
    m_max_it = std::abs(max_it);

    if (m_order == 0) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRtCalibrationAnalytic::init - Order of the correction polynomial must be >0!", __FILE__, __LINE__));
    }

    m_t_length = 1000.0;
    m_t_mean = 500.0;
    // default values, correct values will be set when the input r-t
    // has been given

    m_U = std::vector<CLHEP::HepVector>(m_order);
    m_A = CLHEP::HepSymMatrix(m_order, 0);
    m_b = CLHEP::HepVector(m_order, 0);
    m_alpha = CLHEP::HepVector(m_order, 0);

    // correction function
    if (func_type < 1 || func_type > 3) {
        m_base_function = nullptr;
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRtCalibrationAnalytic::init - Illegal correction function type!", __FILE__, __LINE__));
    }
    switch (func_type) {
        case 1: m_base_function = std::make_unique<LegendrePolynomial>(); break;
        case 2: m_base_function = std::make_unique<ChebyshevPolynomial>(); break;
        case 3:
            if (m_order < 2) {
                throw std::runtime_error(
                    Form("File: %s, Line: %d\nRtCalibrationAnalytic::init - Order must be >2 for polygons! It is set to %i by the user.",
                         __FILE__, __LINE__, m_order));
            }
            std::vector<double> x(m_order);
            double bin_width = 2.0 / static_cast<double>(m_order - 1);
            for (unsigned int k = 0; k < m_order; k++) { x[k] = -1 + k * bin_width; }
            m_base_function = std::make_unique<PolygonBase>(x);
            break;
    }

    // request a chi^2 refit after the quasianalytic pattern recognition //
    m_tracker.switchOnRefit();

    // monotony of r(t) //
    m_force_monotony = false;

    // smoothing //
    m_do_smoothing = do_smoothing;

    // parabolic extrapolation //
    m_do_parabolic_extrapolation = do_parabolic_extrapolation;

    return;
}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD t_from_r ::
//:::::::::::::::::::::
double RtCalibrationAnalytic::t_from_r(const double &r) {
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

//*****************************************************************************

////////////////////////////
// METHOD display_segment //
////////////////////////////
void RtCalibrationAnalytic::display_segment(MuonCalibSegment *segment, std::ofstream &outfile) {
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
    MTStraightLine aux_track(segment->position(), segment->direction(), null, null);
    outfile << "SET PLCI 4\n"
            << "LINE " << aux_track.a_x2() * (z_min - 30.0) + aux_track.b_x2() << " " << z_min - 30.0 << " "
            << aux_track.a_x2() * (z_max + 30.0) + aux_track.b_x2() << " " << z_max + 30.0 << "\n";

    // add a wait statement //
    outfile << "WAIT\n";

    return;
}

//*****************************************************************************

//::::::::::::::::::::::::
//:: METHOD reliability ::
//::::::::::::::::::::::::
double RtCalibrationAnalytic::reliability() const { return m_status; }

//*****************************************************************************

//::::::::::::::::::::::::::::::::
//:: METHOD estimatedRtAccuracy ::
//::::::::::::::::::::::::::::::::
double RtCalibrationAnalytic::estimatedRtAccuracy() const { return m_rt_accuracy; }

//*****************************************************************************

//:::::::::::::::::::::::::::::
//:: METHOD numberOfSegments ::
//:::::::::::::::::::::::::::::
int RtCalibrationAnalytic::numberOfSegments() const { return m_nb_segments; }

//*****************************************************************************

//:::::::::::::::::::::::::::::::::
//:: METHOD numberOfSegmentsUsed ::
//:::::::::::::::::::::::::::::::::
int RtCalibrationAnalytic::numberOfSegmentsUsed() const { return m_nb_segments_used; }

//*****************************************************************************

//::::::::::::::::::::::
//:: METHOD iteration ::
//::::::::::::::::::::::
int RtCalibrationAnalytic::iteration() const { return m_iteration; }

//*****************************************************************************

//:::::::::::::::::::::::::::::::::
//:: METHOD splitIntoMultilayers ::
//:::::::::::::::::::::::::::::::::
bool RtCalibrationAnalytic::splitIntoMultilayers() const { return m_split_into_ml; }

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD fullMatrix ::
//:::::::::::::::::::::::
bool RtCalibrationAnalytic::fullMatrix() const { return m_full_matrix; }

//*****************************************************************************

//:::::::::::::::::::::::
//:: METHOD smoothing ::
//:::::::::::::::::::::::
bool RtCalibrationAnalytic::smoothing() const { return m_do_smoothing; }

//*****************************************************************************

//:::::::::::::::::::::::::::::::::::::::::
//:: METHOD switch_on_control_histograms ::
//:::::::::::::::::::::::::::::::::::::::::
void RtCalibrationAnalytic::switch_on_control_histograms(const std::string &file_name) {
    /////////////////////////////////////////////
    // CREATE THE ROOT FILE AND THE HISTOGRAMS //
    /////////////////////////////////////////////
    m_control_histograms = true;

    m_tfile = std::make_unique<TFile>(file_name.c_str(), "RECREATE");

    m_cut_evolution = std::make_unique<TH1F>("m_cut_evolution", "CUT EVOLUTION", 11, -0.5, 10.5);

    m_nb_segment_hits = std::make_unique<TH1F>("m_nb_segment_hits", "NUMBER OF HITS ON THE REFITTED SEGMENTS", 11, -0.5, 10.5);

    m_CL = std::make_unique<TH1F>("m_CL", "CONFIDENCE LEVELS OF THE REFITTED SEGMENTS", 100, 0.0, 1.0);

    m_residuals = std::make_unique<TH2F>("m_residuals", "RESIDUALS OF THE REFITTED SEGMENTS", 100, -0.5, 15.0, 300, -1.5, 1.5);
}
void RtCalibrationAnalytic::switch_off_control_histograms() {
    m_control_histograms = false;
    if (m_tfile) {
        m_tfile->Write();
        m_tfile.reset();
    }
}

void RtCalibrationAnalytic::forceMonotony() { m_force_monotony = true; }
void RtCalibrationAnalytic::doNotForceMonotony() { m_force_monotony = false; }
void RtCalibrationAnalytic::doSmoothing() { m_do_smoothing = true; }
void RtCalibrationAnalytic::noSmoothing() { m_do_smoothing = false; }
void RtCalibrationAnalytic::doParabolicExtrapolation() { m_do_parabolic_extrapolation = true; }
void RtCalibrationAnalytic::noParabolicExtrapolation() { m_do_parabolic_extrapolation = false; }
void RtCalibrationAnalytic::setEstimateRtAccuracy(const double &acc) { m_rt_accuracy = std::abs(acc); }
void RtCalibrationAnalytic::splitIntoMultilayers(const bool &yes_or_no) { m_split_into_ml = yes_or_no; }
void RtCalibrationAnalytic::fullMatrix(const bool &yes_or_no) { m_full_matrix = yes_or_no; }
RtCalibrationAnalytic::MdtCalibOutputPtr RtCalibrationAnalytic::analyseSegments(const MuonSegVec &seg) {
    std::shared_ptr<const IRtRelation> tmp_rt;
    std::shared_ptr<const IRtRelation> conv_rt;

    //////////////////////////
    // AUTOCALIBRATION LOOP //
    //////////////////////////
    while (!converged()) {
        for (unsigned int k = 0; k < seg.size(); k++) { handleSegment(*seg[k]); }
        if (!analyse()) {
            MsgStream log(Athena::getMessageSvc(), "RtCalibrationAnalytic");
            log << MSG::WARNING << "analyseSegments() - analyse failed, segments:" << endmsg;
            for (unsigned int i = 0; i < seg.size(); i++) {
                log << MSG::WARNING << i << " " << seg[i]->direction() << " " << seg[i]->position() << endmsg;
            }
            return nullptr;
        }

        const RtCalibrationOutput *rtOut = m_output.get();

        if (!converged()) { setInput(rtOut); }
        tmp_rt = rtOut->rt();

        std::vector<double> params;
        params.push_back(tmp_rt->tLower());
        params.push_back(0.01 * (tmp_rt->tUpper() - tmp_rt->tLower()));
        for (double t = tmp_rt->tLower(); t <= tmp_rt->tUpper(); t = t + params[1]) { params.push_back(tmp_rt->radius(t)); }
        conv_rt = std::make_shared<RtRelationLookUp>(params);
    }

    // parabolic extrapolations for small radii //
    if (m_do_parabolic_extrapolation) {
        std::shared_ptr<const RtRelationLookUp> tmprt = performParabolicExtrapolation(true, true, *tmp_rt);
        m_output = std::make_shared<RtCalibrationOutput>(
            tmprt, std::make_shared<RtFullInfo>("RtCalibrationAnalyticExt", m_iteration, m_nb_segments_used, 0.0, 0.0, 0.0, 0.0));
        tmp_rt = tmprt;
    }

    //////////////////////////////////////////////
    // SMOOTHING AFTER CONVERGENCE IF REQUESTED //
    //////////////////////////////////////////////
    if (!m_do_smoothing) { return getResults(); }

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
        unsigned int counter(0);
        unsigned int counter2(0);

        // overwrite drift radii and calculate the average resolution //
        for (unsigned int k = 0; k < seg.size(); k++) {
            if (seg[k]->mdtHitsOnTrack() < 3) { continue; }
            counter2++;
            double avres(0.0);
            for (unsigned int h = 0; h < seg[k]->mdtHitsOnTrack(); h++) {
                seg[k]->mdtHOT()[h]->setDriftRadius(tmp_rt->radius(seg[k]->mdtHOT()[h]->driftTime()),
                                                    seg[k]->mdtHOT()[h]->sigmaDriftRadius());
                if (seg[k]->mdtHOT()[h]->sigmaDriftRadius() < 0.5 * m_r_max) {
                    avres = avres + seg[k]->mdtHOT()[h]->sigma2DriftRadius();
                } else {
                    avres = avres + 0.1;
                }
            }
            avres = avres / static_cast<double>(seg[k]->mdtHitsOnTrack());
            avres = std::sqrt(avres);
            if (seg[k]->mdtHitsOnTrack() > 3) {
                if (smoothing.addResidualsFromSegment(*seg[k], true, 7.0 * avres)) { counter++; }
            } else {
                if (smoothing.addResidualsFromSegment(*seg[k], false, 7.0 * avres)) { counter++; }
            }
        }

        // break, do no smoothing if there are not enough segments //
        if (counter < 1000) {
            MsgStream log(Athena::getMessageSvc(), "RtCalibrationAnalytic");
            log << MSG::WARNING << "analyseSegments() - no smoothing applied due to too small number of reconstructed segments" << endmsg;
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
        tmp_rt, std::make_shared<RtFullInfo>("RtCalibrationAnalytic", m_iteration, m_nb_segments_used, 0.0, 0.0, 0.0, 0.0));

    /////////////////////////////////////////
    // RETURN THE RESULT AFTER CONVERGENCE //
    /////////////////////////////////////////
    return getResults();
}

//*****************************************************************************

//::::::::::::::::::::::::::
//:: METHOD handleSegment ::
//::::::::::::::::::::::::::
bool RtCalibrationAnalytic::handleSegment(MuonCalibSegment &seg) {
    /////////////////////////////////////////////////
    // RETURN, IF THE SEGMENT HAD LESS THAN 3 HITS //
    /////////////////////////////////////////////////
    if (m_control_histograms) { m_cut_evolution->Fill(0.0, 1.0); }

    m_nb_segments = m_nb_segments + 1;
    if (seg.mdtHitsOnTrack() < 3) { return true; }

    if (m_control_histograms) { m_cut_evolution->Fill(1.0, 1.0); }

    if (std::isnan(seg.direction().x()) || std::isnan(seg.direction().y()) || std::isnan(seg.direction().z()) ||
        std::isnan(seg.position().x()) || std::isnan(seg.position().y()) || std::isnan(seg.position().z()))
        return true;
    if (std::abs(seg.direction().y()) > 100) return true;

    ///////////////
    // VARIABLES //
    ///////////////

    // segment reconstruction and segment selection //
    double aux_res;            // auxiliary resolution
    double av_res(0.0);        // average spatial resolution of the tube hits
    double chi2_scale_factor;  // chi^2 scale factor used to take into
                               // account bad r-t accuracy in the segment selection
    IMdtSegmentFitter::HitSelection hit_selection[2];
    hit_selection[0] = IMdtSegmentFitter::HitSelection(seg.mdtHitsOnTrack());
    hit_selection[1] = IMdtSegmentFitter::HitSelection(seg.mdtHitsOnTrack());
    // hit selection vectors for refits in the first and second multilayer
    unsigned int nb_hits_in_ml[2];       // number of hits in the multilayers
    double x;                            // reduced time = (r(t)-0.5*m_r_max)/(0.5*m_r_max)
    std::vector<double> d_track;         // signed distances of the track from the anode wires of the tubes
    std::vector<double> residual_value;  // residuals
    std::vector<MTStraightLine> w;       // anode wires
    Amg::Vector3D null(0.0, 0.0, 0.0);   // auxiliary 0 vector
    Amg::Vector3D xhat(1.0, 0.0, 0.0);   // auxiliary unit vector

    // objects needed to calculate the autocalibration matrix and vector //
    CLHEP::HepVector G;        // vector used in the calculation of the autocalibration matrix
    std::vector<double> zeta;  // vector used in the calculation of G

    ///////////////////////////////////////
    // PREPARATION FOR THE SEGMENT REFIT //
    ///////////////////////////////////////

    // debug display //
    // 	display_segment(&seg, display);

    // overwrite the drift radii according to the input r-t relationship, //
    // calculate the average spatial resolution to define a road width,   //
    // select the hits in the multilayer, if requested                    //
    nb_hits_in_ml[0] = 0;
    nb_hits_in_ml[1] = 0;
    for (unsigned int k = 0; k < seg.mdtHitsOnTrack(); k++) {
        // make the resolution at small radii large enough //
        aux_res = seg.mdtHOT()[k]->sigmaDriftRadius();
        if (m_rt->radius(seg.mdtHOT()[k]->driftTime()) < 0.5) {
            if (aux_res < 0.5 - m_rt->radius(seg.mdtHOT()[k]->driftTime())) { aux_res = 0.5 - m_rt->radius(seg.mdtHOT()[k]->driftTime()); }
        }

        // overwrite radius //
        seg.mdtHOT()[k]->setDriftRadius(m_rt->radius(seg.mdtHOT()[k]->driftTime()), aux_res);
        // 		seg.mdtHOT()[k]->setDriftRadius(m_rt->radius(
        // 					seg.mdtHOT()[k]->driftTime()),
        // 					seg.mdtHOT()[k]->sigmaDriftRadius());

        // average resolution in the segment //
        if (seg.mdtHOT()[k]->sigmaDriftRadius() < 0.5 * m_r_max) {
            av_res = av_res + std::pow(seg.mdtHOT()[k]->sigmaDriftRadius(), 2);
        } else {
            av_res = av_res + 0.01;
        }

        // hit selection //
        (hit_selection[0])[k] = 0;

        // 1st multilayer, if splitting is requested //
        if (m_split_into_ml) { (hit_selection[0])[k] = seg.mdtHOT()[k]->identify().mdtMultilayer() == 2; }

        // 2nd multilayer, if splitting is requested //
        if (m_split_into_ml) { (hit_selection[1])[k] = seg.mdtHOT()[k]->identify().mdtMultilayer() == 1; }

        // reject hits with bad radii or bad resolution //
        if (seg.mdtHOT()[k]->driftRadius() < 0.0 || seg.mdtHOT()[k]->driftRadius() > m_r_max ||
            seg.mdtHOT()[k]->sigmaDriftRadius() > 10.0 * m_r_max) {
            (hit_selection[0])[k] = 1;
            (hit_selection[1])[k] = 1;
        }

        // counting of selected segments //
        nb_hits_in_ml[0] = nb_hits_in_ml[0] + (1 - (hit_selection[0])[k]);
        nb_hits_in_ml[1] = nb_hits_in_ml[1] + (1 - (hit_selection[1])[k]);

        // check for hits in both multilayers (needed if splitting is requested) //
        if (m_multilayer[0] == false && seg.mdtHOT()[k]->identify().mdtMultilayer() == 1) { m_multilayer[0] = true; }
        if (m_multilayer[1] == false && seg.mdtHOT()[k]->identify().mdtMultilayer() == 2) { m_multilayer[1] = true; }
    }

    // average resolution and chi^2 scale factor //
    av_res = std::sqrt(av_res / static_cast<double>(seg.mdtHitsOnTrack()));
    chi2_scale_factor = std::sqrt(av_res * av_res + m_rt_accuracy * m_rt_accuracy) / av_res;

    ///////////////////////////////////////
    // FILL THE AUTOCALIBRATION MATRICES //
    ///////////////////////////////////////

    // set the road width for the track reconstruction //
    m_tracker.setRoadWidth(7.0 * std::sqrt(av_res * av_res + m_rt_accuracy * m_rt_accuracy));

    // loop over the multilayers //

    //-----------------------------------------------------------------------------
    for (int k = 0; k < 1 + m_split_into_ml; k++) {
        //-----------------------------------------------------------------------------

        if (nb_hits_in_ml[k] < 3) { continue; }

        // refit the segments within the multilayers //
        MTStraightLine track;

        if (!m_tracker.fit(seg, hit_selection[k], track)) { continue; }

        if (std::isnan(track.a_x1()) || std::isnan(track.a_x2()) || std::isnan(track.b_x1()) || std::isnan(track.b_x2())) { continue; }

        // check the quality of the fit //
        if (track.chi2PerDegreesOfFreedom() > 5 * chi2_scale_factor) { continue; }

        // check whether there are at least three track hits //
        if (m_control_histograms) { m_nb_segment_hits->Fill(track.numberOfTrackHits(), 1.0); }
        if (track.numberOfTrackHits() < 3) { continue; }

        // reject tracks with silly parameters
        if (std::abs(track.a_x2()) > 8e8) continue;

        // for filling into data class
        if (m_iteration == 0) {
            m_track_slope.Insert(track.a_x2());
            m_track_position.Insert(track.b_x2());
        }

        // bookkeeping for convergence decision and reliability estimation //
        m_chi2 = m_chi2 + track.chi2PerDegreesOfFreedom();
        m_nb_segments_used = m_nb_segments_used + 1;

        // fill the autocalibration objects //
        // auxiliary variables //
        d_track = std::vector<double>(track.numberOfTrackHits());
        residual_value = std::vector<double>(track.numberOfTrackHits());
        w = std::vector<MTStraightLine>(track.numberOfTrackHits());
        G = CLHEP::HepVector(track.numberOfTrackHits());
        zeta = std::vector<double>(track.numberOfTrackHits());

        // base function values //
        for (unsigned int l = 0; l < m_order; l++) {
            m_U[l] = CLHEP::HepVector(track.numberOfTrackHits());
            for (unsigned int m = 0; m < track.numberOfTrackHits(); m++) {
                x = (track.trackHits()[m]->driftRadius() - 0.5 * m_r_max) / (0.5 * m_r_max);
                (m_U[l])[m] = m_base_function->value(l, x);
            }
        }

        // get the wire coordinates, track distances, and residuals //
        for (unsigned int l = 0; l < track.numberOfTrackHits(); l++) {
            w[l] =
                MTStraightLine(Amg::Vector3D(0.0, (track.trackHits()[l]->localPosition()).y(), (track.trackHits()[l]->localPosition()).z()),
                               xhat, null, null);
            d_track[l] = track.signDistFrom(w[l]);
            residual_value[l] = track.trackHits()[l]->driftRadius() - std::abs(d_track[l]);
            if (m_control_histograms) { m_residuals->Fill(std::abs(d_track[l]), residual_value[l], 1.0); }
        }

        // loop over all track hits //
        for (unsigned int l = 0; l < track.numberOfTrackHits(); l++) {
            // analytic calculation of G //
            for (unsigned int m = 0; m < track.numberOfTrackHits(); m++) {
                zeta[m] = std::sqrt(1.0 + std::pow(track.a_x2(), 2)) * (w[m].positionVector()).z() - track.a_x2() * d_track[m];
            }
            for (unsigned int m = 0; m < track.numberOfTrackHits(); m++) {
                double sum1(0.0), sum2(0.0), sum3(0.0), sum4(0.0);
                for (unsigned int ll = 0; ll < track.numberOfTrackHits(); ll++) {
                    sum1 = sum1 + (zeta[l] - zeta[ll]) * (zeta[ll] - zeta[m]) /
                                      (track.trackHits()[m]->sigma2DriftRadius() * track.trackHits()[ll]->sigma2DriftRadius());
                    sum2 = sum2 + zeta[ll] / track.trackHits()[ll]->sigma2DriftRadius();
                    sum3 = sum3 + 1.0 / track.trackHits()[ll]->sigma2DriftRadius();
                    sum4 = sum4 + std::pow(zeta[ll] / track.trackHits()[ll]->sigmaDriftRadius(), 2);
                }
                if (d_track[m] * d_track[l] >= 0.0) {
                    G[m] = (l == m) - m_full_matrix * sum1 / (sum2 * sum2 - sum3 * sum4);
                } else {
                    G[m] = (l == m) + m_full_matrix * sum1 / (sum2 * sum2 - sum3 * sum4);
                }
            }
            CLHEP::HepSymMatrix A_tmp(m_A);
            // autocalibration objects //
            for (unsigned int p = 0; p < m_order; p++) {
                for (unsigned int pp = p; pp < m_order; pp++) {
                    A_tmp[p][pp] = m_A[p][pp] + (dot(G, m_U[p]) * dot(G, m_U[pp])) / track.trackHits()[l]->sigma2DriftRadius();
                    if (std::isnan(A_tmp[p][pp])) return true;
                }
            }
            m_A = A_tmp;
            CLHEP::HepVector b_tmp(m_b);
            for (unsigned int p = 0; p < m_order; p++) {
                b_tmp[p] = m_b[p] - residual_value[l] * dot(G, m_U[p]) / track.trackHits()[l]->sigma2DriftRadius();
                if (std::isnan(b_tmp[p])) return true;
            }
            m_b = b_tmp;
        }

        //-----------------------------------------------------------------------------
    }
    //-----------------------------------------------------------------------------
    return true;
}

//*****************************************************************************

//:::::::::::::::::::::
//:: METHOD setInput ::
//:::::::::::::::::::::
void RtCalibrationAnalytic::setInput(const IMdtCalibrationOutput *rt_input) {
    ///////////////
    // VARIABLES //
    ///////////////
    const RtCalibrationOutput *input(dynamic_cast<const RtCalibrationOutput *>(rt_input));

    ////////////////////////////////////////////
    // CHECK IF THE OUTPUT CLASS IS SUPPORTED //
    ////////////////////////////////////////////
    if (input == nullptr) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRtCalibrationAnalytic::setInput - Calibration input class not supported.", __FILE__, __LINE__));
    }

    /////////////////////////////////////////////////////////////////
    // GET THE INITIAL r-t RELATIONSHIP AND RESET STATUS VARIABLES //
    /////////////////////////////////////////////////////////////////

    // get the r-t relationship //
    m_rt = input->rt();
    m_r_max = m_rt->radius(m_rt->tUpper());

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
        throw std::runtime_error(
            Form("File: %s, Line: %d\nRtCalibrationAnalytic::setInput - r-t class not supported.", __FILE__, __LINE__));
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
}

bool RtCalibrationAnalytic::analyse() {
    if (m_tfile) m_tfile->cd();

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
    RtParabolicExtrapolation rt_extrapolator;    // r-t extrapolator
    RtFromPoints rt_from_points;                 // r-t from points

    ////////////////////////////////////////
    // SOLVE THE AUTOCALIBRATION EQUATION //
    ////////////////////////////////////////
    m_alpha = m_A.inverse(ifail) * m_b;
    if (ifail != 0) {
        MsgStream log(Athena::getMessageSvc(), "RtCalibrationAnalytic");
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
            if (((k == 0 || x_r[k].x2() < 0.5) && m_fix_min) || ((k == nb_points || x_r[k].x2() > 14.1) && m_fix_max)) {
                r_corr = 0.0;
                x_r[k].set_error(0.01);
            }
            x_r[k].set_x2(x_r[k].x2() + r_corr);
        }

        // force monotony //
        if (m_force_monotony) {
            for (unsigned int k = 0; k < nb_points; k++) {
                if (x_r[k].x2() > x_r[k + 1].x2()) { x_r[k + 1].set_x2(x_r[k].x2()); }
            }
        }

        if (m_control_histograms) {
            std::unique_ptr<TGraphErrors> gre = std::make_unique<TGraphErrors>(x_r.size());
            for (unsigned int i = 0; i < 1; i++) {
                gre->SetPoint(i, x_r[i].x1(), x_r[i].x2());
                gre->SetPointError(i, 0, x_r[i].error());
            }
            std::ostringstream str_str;
            str_str << "CorrectionPoints_" << m_iteration;
            gre->Write(str_str.str().c_str());
        }

        // create the new r-t relationship //
        fitter.fit_parameters(x_r, 1, nb_points + 1, &chebyshev);
        rt_param[0] = rt_Chebyshev->tLower();
        rt_param[1] = rt_Chebyshev->tUpper();
        for (unsigned int k = 0; k < rt_Chebyshev->numberOfRtParameters(); k++) { rt_param[k + 2] = fitter.coefficients()[k]; }

        m_rt_new = std::make_unique<RtChebyshev>(rt_param);
    }

    // input-rt is of type RtRelationLookUp //
    if (rt_LookUp) {
        rt_param = rt_LookUp->parameters();
        unsigned int min_k(2), max_k(rt_param.size());
        if (m_fix_min) { min_k = 3; }
        if (m_fix_max) { max_k = rt_param.size() - 1; }

        std::unique_ptr<TGraph> gr;
        if (m_control_histograms) gr = std::make_unique<TGraph>(max_k - min_k);

        for (unsigned int k = min_k; k < max_k; k++) {
            x = (rt_param[k] - 0.5 * m_r_max) / (0.5 * m_r_max);
            r_corr = m_alpha[0];
            for (unsigned int l = 1; l < m_order; l++) { r_corr = r_corr + m_alpha[l] * m_base_function->value(l, x); }
            rt_param[k] = rt_param[k] + r_corr;
            if (m_control_histograms) gr->SetPoint(k - min_k, x, r_corr);
            if (m_force_monotony && k > 2) {
                if (rt_param[k] < rt_param[k - 1]) { rt_param[k] = rt_param[k - 1]; }
            }
        }

        m_rt_new = std::make_unique<RtRelationLookUp>(rt_param);
        if (m_control_histograms) {
            std::ostringstream str_str;
            str_str << "CorrectionPoints_" << m_iteration;
            gr->Write(str_str.str().c_str());
        }
        m_r_max = m_rt_new->radius(m_rt_new->tUpper());
    }

    /////////////////////////////////////////////////////////
    // DETERMINE THE r-t QUALITY AND CHECK FOR CONVERGENCE //
    /////////////////////////////////////////////////////////

    // estimate r-t accuracy //
    m_rt_accuracy = 0.0;
    double r_corr_max = 0.0;

    for (unsigned int k = 0; k < 100; k++) {
        r_corr = m_alpha[0];
        for (unsigned int l = 1; l < m_order; l++) { r_corr = r_corr + m_alpha[l] * m_base_function->value(l, -1.0 + 0.01 * k); }
        if (std::abs(r_corr) > r_corr_max) { r_corr_max = std::abs(r_corr); }
        m_rt_accuracy = m_rt_accuracy + r_corr * r_corr;
    }
    m_rt_accuracy = std::sqrt(0.01 * m_rt_accuracy);
    m_rt_accuracy_previous = m_rt_accuracy;

    // convergence? //

    m_chi2 = m_chi2 / static_cast<double>(m_nb_segments_used);
    if (((m_chi2 < m_chi2_previous && std::abs(m_chi2 - m_chi2_previous) > 0.01) || std::abs(m_rt_accuracy) > 0.001) &&
        m_iteration < m_max_it) {
        m_status = 0;  // no convergence yet
    } else {
        m_status = 1;
    }
    m_chi2_previous = m_chi2;

    // reliabilty of convergence //
    if (m_status != 0) {
        if (m_split_into_ml && m_nb_segments_used < 0.5 * (m_multilayer[0] + m_multilayer[1]) * m_nb_segments) { m_status = 2; }
        if (!m_split_into_ml && m_nb_segments_used < 0.5 * m_nb_segments) { m_status = 2; }
    }

    //////////////////////////////////////////////////
    // STORE THE RESULTS IN THE RtCalibrationOutput //
    //////////////////////////////////////////////////
    m_iteration = m_iteration + 1;

    m_output = std::make_shared<RtCalibrationOutput>(
        m_rt_new, std::make_shared<RtFullInfo>("RtCalibrationAnalytic", m_iteration, m_nb_segments_used, 0.0, 0.0, 0.0, 0.0));

    return true;
}
bool RtCalibrationAnalytic::converged() const { return (m_status > 0); }
RtCalibrationAnalytic::MdtCalibOutputPtr RtCalibrationAnalytic::getResults() const { return m_output; }
std::shared_ptr<RtRelationLookUp> RtCalibrationAnalytic::performParabolicExtrapolation(const bool &min, const bool &max,
                                                                                       const IRtRelation &in_rt) {
    ///////////////
    // VARIABLES //
    ///////////////
    RtParabolicExtrapolation rt_extrapolator;           // r-t extrapolator
    std::shared_ptr<RtRelationLookUp> rt_low, rt_high;  // pointers to the r-t
                                                        // relationships after
                                                        // extrapolation
    std::vector<SamplePoint> add_fit_point;             // additional r-t points used if r(0) or
                                                        // r(t_max) is fixed.

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
            rt_high = std::make_shared<RtRelationLookUp>(
                rt_extrapolator.getRtWithParabolicExtrapolation(in_rt, m_r_max - 3.0, m_r_max - 1.0, m_r_max, add_fit_point));
        }
    }

    ////////////////////////////////
    // EXTRAPOLATE TO SMALL RADII //
    ////////////////////////////////
    if (min) {
        add_fit_point.clear();
        if (m_fix_min) { add_fit_point.push_back(SamplePoint(m_rt_new->tLower(), 0.0, 1.0)); }
        if (m_fix_max && rt_high) {
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
    if (min && max) { return rt_low; }
    if (min) { return rt_low; }
    return rt_high;
}
