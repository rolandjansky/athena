/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughTransformer_xy.h"

MuonHoughTransformer_xy::MuonHoughTransformer_xy(int nbins, int nbins_angle, double detectorsize, double detectorsize_angle,
                                                 double threshold_histo, int number_of_sectors) :
    MuonHoughTransformer_xyz(nbins, nbins_angle, detectorsize, detectorsize_angle, threshold_histo, number_of_sectors) {}

std::pair<double, double> MuonHoughTransformer_xy::getHitPos(const MuonHoughHitContainer* event, int hitid)
    const  // returns the relevant position of the hit (xy-RPC in case of id_number==id_xy_rpc etc.)
{
    std::pair<double, double> hitpos;
    hitpos.first = event->getHitx(hitid);
    hitpos.second = event->getHity(hitid);
    return hitpos;
}

MuonHoughPattern* MuonHoughTransformer_xy::initialiseHoughPattern() const {
    MuonHoughPattern* houghpattern = new MuonHoughPattern(MuonHough::hough_xy);
    return houghpattern;
}

float MuonHoughTransformer_xy::weightHoughTransform(double r0) const { return MuonHoughTransformer_xyz::weightHoughTransform(r0); }
