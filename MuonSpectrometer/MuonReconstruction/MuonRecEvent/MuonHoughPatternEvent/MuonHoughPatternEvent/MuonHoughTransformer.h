/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_H

#include <cmath>
#include <map>

#include "MuonHoughPatternEvent/MuonHoughHisto2DContainer.h"
#include "MuonHoughPatternEvent/MuonHoughHit.h"
#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"
#include "MuonHoughPatternEvent/MuonHoughPattern.h"

class MuonHoughHitContainer;

/** Abstract base class, Strategy pattern */
/** from this class, algorithms inherit and do the houghtransformation and searches for a houghpattern */

class MuonHoughTransformer {
public:
    /** destructor */
    virtual ~MuonHoughTransformer();

    /** weight houghtransform, give more importance to houghtransforms close to origin */
    virtual float weightHoughTransform(double r0) const = 0;
    /** fill histograms with hitcontainer */
    virtual void fill(const MuonHoughHitContainer* event, bool subtract = false);
    /** fill histograms with hit */
    virtual void fillHit(MuonHoughHit* hit, double weight = 1.) = 0;
    /** fill histogram with certain coordinate */
    virtual int fillHisto(double coord1, double coord2, double weight = 1., int sector = 0) = 0;

    /** associate hits to certain maximum number of histograms */
    MuonHoughPattern* associateHitsToMaximum(const MuonHoughHitContainer* event, double residu_mm, double residu_grad, int maximum_number,
                                             bool which_segment = 0, int printlevel = 0) const;

    /** associate hits to certain coordinates and sector */
    MuonHoughPattern* associateHitsToCoords(const MuonHoughHitContainer* event, std::pair<double, double> coords, double residu_mm,
                                            double residu_angle, int sector = 0, bool which_segment = 0,
                                            int printlevel = 0) const;  // residu_mm for id=0,1,2 ; residu_angle for id=3,4

    /** associate hits to certain binnumber and sector */
    MuonHoughPattern* associateHitsToBinnumber(const MuonHoughHitContainer* event, int binnumber, double maximum_residu_mm,
                                               double maximum_residu_angle, int sector = 0, bool which_segment = 0,
                                               int printlevel = 0) const;

    /** reset histograms */
    void resetHisto();
    /** use negative weights */
    void useNegativeWeights(bool use_negative_weights);

    /** return the first certain number of maxima of histograms */
    std::vector<std::pair<int, int> > getMaxima(int max_patterns) const;

    /** set m_ip_setting (e.g. to not split patterns) */
    void setIP(bool ip_setting);

    /** access to histograms */
    const MuonHoughHisto2DContainer& histos() const { return m_histos; }

protected:
    /** constructor, input values are those of histograms */
    MuonHoughTransformer(int nbins, int nbins_angle, double detectorsize, double detectorsize_angle, double threshold_histo,
                         int number_of_sectors = 1);

    /** pure virtual method for derived class implementation of associateHitsToMaximum method */
    virtual MuonHoughPattern* hookAssociateHitsToMaximum(const MuonHoughHitContainer* event, std::pair<double, double> coordsmaximum,
                                                         double residu_mm, double residu_angle, int sector = 0, bool which_segment = 0,
                                                         int printlevel = 0) const = 0;

    /** returns begin and end value of the filling loop */
    std::pair<double, double> getEndPointsFillLoop(double radius, double stepsize, int sector = 0) const;

    /** returns sector for coords */
    virtual int sector(MuonHoughHit* hit) const = 0;

    /** returns sinus from lookup table */
    double sinus(double angle) const;
    /** returns cosinus from lookup table */
    double cosinus(double angle) const;

    /** histogram container */
    MuonHoughHisto2DContainer m_histos;

    /** threshold of histograms */
    double m_threshold_histo;

    /** size of event to be filled (can be used for weighting) */
    unsigned int m_eventsize;
    /** weightfactor based on eventsize (used in curved hough transform) */
    double m_eventsize_weightfactor;

    /** use weight of patterns in angle coordinate */
    bool m_add_weight_angle;
    /** weight constant of patterns in angle coordinate */
    double m_weight_constant_angle;
    /** use weight of patterns in radius coordinate */
    bool m_add_weight_radius;
    /** weight constant of patterns in radius coordinate */
    double m_weight_constant_radius;

    /** use of negative weights */
    bool m_use_negative_weights;

    /** object for use of mathematical formulas for trackmodels */
    MuonHoughMathUtils m_muonhoughmathutils;

    /** number of bins in histograms in radius coordinate*/
    const int m_nbins;
    /** number of bins in histograms in radius coordinate*/
    const int m_nbins_plus3;
    /** number of bins in histograms in angle coordinate*/
    const int m_nbins_angle;
    /** range of radius coordinate */
    const double m_detectorsize;
    /** range of angle coordinate */
    const double m_detectorsize_angle;
    /** x-binwidth of histogram */
    double m_binwidthx;
    /** y-binwidth of histogram */
    double m_binwidthy;

    /** stepsize of transform for radius coordinate */
    double m_stepsize;
    /** stepsize of transform for angle coordinate */
    double m_stepsize_per_angle;

    /** use settings for patterns originating from origin */
    bool m_ip_setting;

    /** number of histograms (1 for cosmics 16 for rz) */
    const int m_number_of_sectors;

private:
    /** initialize sinus and cosinus tables */
    void initTables();

    /** lookup table <angle in rad, sinus value> for sin */
    std::map<double, double> m_sin;
    /** lookup table <angle in rad, cosinus value> for con */
    std::map<double, double> m_cos;

    class maximaCompare {
    public:
        bool operator()(const std::pair<std::pair<int, int>, double>& lhs, const std::pair<std::pair<int, int>, double>& rhs) const {
            return lhs.second > rhs.second;
        }
    };
};

inline void MuonHoughTransformer::setIP(bool ip_setting) { m_ip_setting = ip_setting; }
inline void MuonHoughTransformer::useNegativeWeights(bool use_negative_weights) { m_use_negative_weights = use_negative_weights; }

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_H
