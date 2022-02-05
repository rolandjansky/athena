/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_CURVEDATACYLINDER_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_CURVEDATACYLINDER_H

#include "MuonHoughPatternEvent/MuonHoughTransformer.h"

class MuonHoughTransformer_CurvedAtACylinder : public MuonHoughTransformer {
public:
    /** constructor */
    MuonHoughTransformer_CurvedAtACylinder(int nbins, int nbins_angle, double detectorsize, double detectorsize_angle,
                                           double threshold_histo, int number_of_sectors = 1);
    /** destructor */
    ~MuonHoughTransformer_CurvedAtACylinder();

    /** fill hit in histogram */
    virtual void fillHit(MuonHoughHit* hit, double weight = 1.);
    /** fill transformed values in histogram */
    virtual int fillHisto(double xbin, double theta, double weight = 1., int sector = 0);

    /** associate hits to maximum found */
    virtual MuonHoughPattern* hookAssociateHitsToMaximum(const MuonHoughHitContainer* event, std::pair<double, double> coordsmaximum,
                                                         double residu_mm, double residu_grad, int sector = 0, bool which_segment = 0,
                                                         int printlevel = 999) const;

    /** returns the phi sector */
    virtual int sector(MuonHoughHit* hit) const;
    /** not implemented for this transform */
    virtual float weightHoughTransform(double r0) const;

private:
    /** array that stores the inverse curvatures that are scanned */
    double* m_invcurvature;
    double* m_weightcurvature;

    MuonHoughTransformer_CurvedAtACylinder& operator=(const MuonHoughTransformer_CurvedAtACylinder& right);
    MuonHoughTransformer_CurvedAtACylinder(const MuonHoughTransformer_CurvedAtACylinder&);
};

inline int MuonHoughTransformer_CurvedAtACylinder::sector(MuonHoughHit* hit) const { return hit->phiSector(); }

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_CURVEDATACYLINDER_H
