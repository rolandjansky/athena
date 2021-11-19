/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_XY_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_XY_H

#include "MuonHoughPatternEvent/MuonHoughTransformer_xyz.h"

class MuonHoughTransformer_xy : public MuonHoughTransformer_xyz {
public:
    /** constructor */
    MuonHoughTransformer_xy(int nbins, int nbins_angle, double detectorsize, double detectorsize_angle, double threshold_histo,
                            int number_of_sectors = 1);
    /** destructor */
    virtual ~MuonHoughTransformer_xy() = default;

    /** returns the hit position in xy frame */
    virtual std::pair<double, double> getHitPos(const MuonHoughHitContainer* event, int hitid) const;
    /** build new houghpattern */
    virtual MuonHoughPattern* initialiseHoughPattern() const;
    /** put weight on houghtransform dependent on r0 */
    virtual float weightHoughTransform(double r0) const;
};

#endif  // MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMER_XY_H
