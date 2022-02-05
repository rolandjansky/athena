/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibRt/AdaptiveResidualSmoothing.h"

#include <TString.h>  // for Form

#include <algorithm>
#include <fstream>
#include <iostream>

#include "AthenaKernel/getMessageSvc.h"
#include "CLHEP/Matrix/Vector.h"
#include "GaudiKernel/MsgStream.h"
#include "MdtCalibData/IRtRelation.h"
#include "MuonCalibMath/BaseFunctionFitter.h"
#include "MuonCalibMath/ConstantContentBinMaker.h"
#include "MuonCalibMath/DataPoint.h"
#include "MuonCalibMath/PolygonBase.h"
#include "TSpline.h"
using namespace CLHEP;
using namespace MuonCalib;

AdaptiveResidualSmoothing::AdaptiveResidualSmoothing() {}
void AdaptiveResidualSmoothing::clear() { m_residual_point.clear(); }
void AdaptiveResidualSmoothing::addResidual(const double &radius, const double &residual) {
    // make the data point //
    CLHEP::HepVector point(2);
    point[0] = radius;
    point[1] = residual;
    DataPoint residual_point(point, 0);

    // store the data point //
    m_residual_point.push_back(residual_point);

    return;
}
bool AdaptiveResidualSmoothing::addResidualsFromSegment(MuonCalibSegment &seg, bool curved, double road_width) {
    ///////////////
    // VARIABLES //
    ///////////////

    CLHEP::HepVector point(2);  // auxiliary point
    IMdtPatRecFitter *fitter = nullptr;

    ////////////////////////////////////////
    // RESIDUAL DETERMINATION AND STORAGE //
    ////////////////////////////////////////

    // perform fit //
    if (curved) {
        // set road width /
        m_cfitter.setRoadWidth(road_width);

        // set fitter pointer //
        fitter = &m_cfitter;

    } else {
        // set road width /
        m_sfitter.setRoadWidth(road_width);

        // set fitter pointer //
        fitter = &m_sfitter;
    }

    fitter->SetRefineSegmentFlag(false);

    CurvedLine curved_track;
    MTStraightLine straight_track;
    IMdtSegmentFitter::HitSelection selection(seg.mdtHitsOnTrack(), 0);

    // perform the track fit; return in case of failure //

    // reject bad fits //
    if (curved) {
        if (!m_cfitter.fit(seg, selection, curved_track)) return false;
        if (curved_track.chi2PerDegreesOfFreedom() > 50) { return false; }
    } else {
        if (!m_sfitter.fit(seg, selection, straight_track)) return false;
        if (straight_track.chi2PerDegreesOfFreedom() > 50) { return false; }
    }

    // store the residuals //
    const MuonCalibSegment::MdtHitVec &trackHits = curved ? curved_track.trackHits() : straight_track.trackHits();
    for (unsigned int k = 0; k < trackHits.size(); k++) {
        point[0] = trackHits[k]->driftRadius();
        point[1] = trackHits[k]->driftRadius() - std::abs(trackHits[k]->signedDistanceToTrack());
        m_residual_point.emplace_back(point, 0);
    }

    return true;
}

RtRelationLookUp AdaptiveResidualSmoothing::performSmoothing(const IRtRelation &rt_rel, unsigned int nb_entries_per_bin, bool fix_t_min,
                                                             bool fix_t_max) {
    ///////////////
    // VARIABLES //
    ///////////////

    ConstantContentBinMaker bin_maker(m_residual_point, 0.001);
    std::vector<unsigned int> ref_coord(1);

    ////////////
    // CHECKS //
    ////////////

    if (m_residual_point.size() == 0) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nAdaptiveResidualSmoothing::performSmoothing - No residuals stored.", __FILE__, __LINE__));
    }

    if (m_residual_point.size() < nb_entries_per_bin) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nAdaptiveResidualSmoothing::performSmoothing - Not enough residuals stored.", __FILE__, __LINE__));
    }

    /////////////////////
    // PERFORM BINNING //
    /////////////////////

    ref_coord[0] = 0;
    bin_maker.binDataPoints(nb_entries_per_bin, ref_coord);

    //////////////////////////////
    // DETERMINE r-t COORECTION //
    //////////////////////////////

    // get a polygon through the bin centres //
    std::vector<double> rad(bin_maker.getBins().size());
    std::vector<SamplePoint> corr(bin_maker.getBins().size());
    for (unsigned int k = 0; k < bin_maker.getBins().size(); k++) {
        rad[k] = (bin_maker.getBins()[k])->centreOfGravity()[0];
        corr[k].set_x1(rad[k]);
        corr[k].set_x2((bin_maker.getBins()[k])->centreOfGravity()[1]);
        corr[k].set_error(1.0);
    }
    std::sort(rad.begin(), rad.end());

    std::vector<double> radi;
    radi.push_back(rad[0]);
    unsigned int counter(0);
    for (unsigned int k = 1; k < rad.size(); k++) {
        if (rad[counter] < rad[k]) {
            radi.push_back(rad[k]);
            counter++;
        }
    }

    PolygonBase polygon(radi);
    BaseFunctionFitter fitter(bin_maker.getBins().size());
    fitter.fit_parameters(corr, 1, corr.size(), &polygon);

    // create an improved r-t relationship //
    std::vector<double> rt_params;
    rt_params.push_back(rt_rel.tLower());
    rt_params.push_back(0.01 * (rt_rel.tUpper() - rt_rel.tLower()));
    //	ofstream outfile("out2.txt");
    for (double t = rt_rel.tLower(); t <= rt_rel.tUpper(); t = t + rt_params[1]) {
        double delta_r(0.0);
        for (unsigned int l = 0; l < radi.size(); l++) {
            delta_r = delta_r + fitter.coefficients()[l] * polygon.value(l, rt_rel.radius(t));
        }
        if (fix_t_min && (rt_rel.radius(t) < 0.5)) { delta_r = 0.0; }
        if (fix_t_max && (rt_rel.radius(t) > 14.1)) { delta_r = 0.0; }
        rt_params.push_back(rt_rel.radius(t) - delta_r);
    }

    RtRelationLookUp improved_rt(rt_params);
    // 	for (double t=rt_rel.tLower(); t<=rt_rel.tUpper(); t=t+rt_params[1]) {
    // 		outfile << rt_rel.radius(t) << "\t" << improved_rt.radius(t)
    // 				<< "\t" << endl;
    // 	}

    return improved_rt;
}

RtRelationLookUp AdaptiveResidualSmoothing::performSmoothing(const IRtRelation &rt_rel, const bool &fix_t_min, const bool &fix_t_max) {
    ///////////////
    // VARIABLES //
    ///////////////

    unsigned int nb_bins;                     // number of bins in r
    unsigned int min_nb_entries_per_bin(20);  // minimum number of
                                              // entries per bin
    double step_size;                         // real step size [mm]
    double r_max(16.0);                       // maximum drift radius
    double aux_rad;                           // auxiliary radius
    double aux_corr;                          // auxiliary correction value
    unsigned int bin_index;

    //////////////////////////////////
    // DETERMINE THE NUMBER OF BINS //
    //////////////////////////////////

    // take the sqrt from the number of residuals to determine the binsize.
    // for low stat, a larger statistical error on the bins is allowed
    // for example:
    // 1000 segments -> 7.6% and 35 bins
    // 5000 segments -> 5.1% and 64 bins
    // 10000 segments-> 4.1% and 91 bins
    nb_bins = static_cast<unsigned int>(std::sqrt(m_residual_point.size() / 6.));

    // calculate min_nb_per_bin
    min_nb_entries_per_bin = m_residual_point.size() / nb_bins;

    //////////////////////////////////////
    // RETURN IF THERE ARE NO RESIDUALS //
    //////////////////////////////////////

    if (m_residual_point.size() == 0) {
        MsgStream log(Athena::getMessageSvc(), "AdaptiveResidualSmoothing");
        log << MSG::WARNING << "performSmoothing() - No residuals are stored. no correction applied to r(t)!" << endmsg;
    }

    if (nb_bins == 0) { nb_bins = 1; }
    step_size = r_max / static_cast<double>(nb_bins);

    //////////////////////////////////////////////////////////////
    // DETERMINE THE AVERAGE RESIDUAL VALUES IN THE RADIAL BINS //
    //////////////////////////////////////////////////////////////

    // vector for the correction function //
    std::vector<SamplePoint> corr(nb_bins);
    std::vector<double> radii(nb_bins);

    // sort the residuals point in the residual value for a simple outlyer //
    // rejection performed later                                           //
    for (unsigned int k = 0; k < m_residual_point.size(); k++) { m_residual_point[k].setReferenceComponent(1); }
    sort(m_residual_point.begin(), m_residual_point.end());

    // auxiliary data point arrays //
    std::vector<std::vector<const DataPoint *> > sample_points_per_r_bin(nb_bins);

    // group data points in r bins //
    for (unsigned int k = 0; k < m_residual_point.size(); k++) {
        if (std::abs(m_residual_point[k].dataVector()[0]) >= r_max) { continue; }
        bin_index = static_cast<unsigned int>(std::abs(m_residual_point[k].dataVector()[0]) / step_size);
        sample_points_per_r_bin[bin_index].push_back(&(m_residual_point[k]));
    }

    // loop over the r bins and determine the r-t corrections //
    //     static ofstream outfile("tmp.txt");
    //     outfile << endl;
    for (unsigned int bin = 0; bin < nb_bins; bin++) {
        aux_rad = 0.0;
        aux_corr = 0.0;

        // remove the first and last 1% of the residuals as simple outlyer removal //
        unsigned int k_min(static_cast<unsigned int>(0.01 * sample_points_per_r_bin[bin].size()));
        unsigned int k_max(static_cast<unsigned int>(0.99 * sample_points_per_r_bin[bin].size()));
        //         outfile << bin << "\t" << (0.5+bin)*step_size << "\t"
        //                 << k_min << "\t" << k_max << endl;

        // loop over residuals in a bin
        for (unsigned int k = k_min; k < k_max; k++) {
            aux_rad = aux_rad + sample_points_per_r_bin[bin][k]->dataVector()[0];
            aux_corr = aux_corr + sample_points_per_r_bin[bin][k]->dataVector()[1];
        }
        if (k_max - k_min < 0.25 * min_nb_entries_per_bin) {
            radii[bin] = (0.5 + bin) * step_size;
            corr[bin] = SamplePoint(radii[bin], 0.0, 1.0);
        } else {
            radii[bin] = aux_rad / static_cast<double>(k_max - k_min);
            corr[bin] = SamplePoint(radii[bin], aux_corr / static_cast<double>(k_max - k_min), 1.0);
        }
    }

    // correction polygon //
    PolygonBase polygon(radii);
    BaseFunctionFitter fitter(nb_bins);
    fitter.fit_parameters(corr, 1, corr.size(), &polygon);

    // create output r-t relationship //
    std::vector<double> rt_params;
    rt_params.push_back(rt_rel.tLower());
    rt_params.push_back(0.01 * (rt_rel.tUpper() - rt_rel.tLower()));
    //	ofstream outfile("out2.txt");
    for (double t = rt_rel.tLower(); t <= rt_rel.tUpper(); t = t + rt_params[1]) {
        double delta_r(0.0);
        for (unsigned int l = 0; l < radii.size(); l++) {
            delta_r = delta_r + fitter.coefficients()[l] * polygon.value(l, rt_rel.radius(t));
        }
        if (rt_rel.radius(t) > r_max) { delta_r = 0.0; }
        if (fix_t_min && (rt_rel.radius(t) < 0.5)) { delta_r = 0.0; }
        if (fix_t_max && (rt_rel.radius(t) > 14.1)) { delta_r = 0.0; }
        rt_params.push_back(rt_rel.radius(t) - delta_r);
    }

    RtRelationLookUp improved_rt(rt_params);
    // 	for (double t=rt_rel.tLower(); t<=rt_rel.tUpper(); t=t+rt_params[1]) {
    // 		outfile << rt_rel.radius(t) << "\t" << improved_rt.radius(t)
    // 				<< "\t" << endl;
    // 	}

    return improved_rt;
}
double AdaptiveResidualSmoothing::t_from_r(const IRtRelation &rt_rel, const double &r) {
    ///////////////
    // VARIABLES //
    ///////////////

    double precision(0.001);        // spatial precision of the inversion
    double t_max(rt_rel.tUpper());  // upper time search limit
    double t_min(rt_rel.tLower());  // lower time search limit

    /////////////////////////////////////////////
    // SEARCH FOR THE CORRESPONDING DRIFT TIME //
    /////////////////////////////////////////////

    while (t_max - t_min > 0.1 && std::abs(rt_rel.radius(0.5 * (t_min + t_max)) - r) > precision) {
        if (rt_rel.radius(0.5 * (t_min + t_max)) > r) {
            t_max = 0.5 * (t_min + t_max);
        } else {
            t_min = 0.5 * (t_min + t_max);
        }
    }

    return 0.5 * (t_min + t_max);
}
