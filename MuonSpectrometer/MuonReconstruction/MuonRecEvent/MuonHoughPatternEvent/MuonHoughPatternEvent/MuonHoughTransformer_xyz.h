/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_XYZ_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_XYZ_H

#include "MuonHoughPatternEvent/MuonHoughTransformer.h"

/** abstract base class for hough transform with carthesian coordinates */
class MuonHoughTransformer_xyz : public MuonHoughTransformer {
protected:
    /** constructor */
    MuonHoughTransformer_xyz(int nbins, int nbins_angle, double detectorsize, double detectorsize_angle, double threshold_histo,
                             int number_of_sectors = 1);

    /** associate hits to maximum found */
    virtual MuonHoughPattern* hookAssociateHitsToMaximum(const MuonHoughHitContainer* event, std::pair<double, double> coordsmaximum,
                                                         double residu_mm, double residu_grad, int sector = 0, bool which_segment = 0,
                                                         int printlevel = 999) const;

public:
    /** destructor */
    virtual ~MuonHoughTransformer_xyz() = default;

    /** fill hit in histogram */
    virtual void fillHit(MuonHoughHit* hit, double weight = 1.);
    /** fill transformed values in histogram */
    virtual int fillHisto(double r0, double phi, double weight = 1., int sector = 0);

    /** returns the rz sector */
    virtual int sector(MuonHoughHit* hit) const;

    /** calcalates the phi angle for a given hit and r0 */
    static double calculateAngle(double hitx, double hity, double r0);

    /** build new houghpattern */
    virtual MuonHoughPattern* initialiseHoughPattern() const = 0;
    /** put weight on houghtransform dependent on r0 */
    virtual float weightHoughTransform(double r0) const;

    /** returns the relevant 2d hit position */
    virtual std::pair<double, double> getHitPos(const MuonHoughHitContainer* event, int hitid) const = 0;
};

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_XYZ_H
