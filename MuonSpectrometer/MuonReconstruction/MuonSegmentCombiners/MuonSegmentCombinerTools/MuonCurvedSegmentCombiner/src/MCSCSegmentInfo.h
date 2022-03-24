/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCSCSEGMENTINFO_H
#define MCSCSEGMENTINFO_H

#include <iostream>
#include <vector>

namespace Muon {

    class MuonSegment;
    class MuonPattern;
    class MuonPatternCombination;

    struct MCSCSegmentInfo {
        MCSCSegmentInfo() :
            index(-1),
            nCsc(0),
            nHots(0),
            nMissedHits(0),
            nLayTrig(0),
            layMissed(0.),
            chi2(0.),
            nMult1(0),
            nMult2(0),
            phiPatHits(0),
            patHits(0),
            stationCode(0),
            station(""),
            invcurvature(0.),
            patPhi(0.),
            patTheta(0.),
            patMomentum(0.),
            patPoint(0) {}

        int index;
        int nCsc;
        int nHots;
        int nMissedHits;
        int nLayTrig;
        double layMissed;
        double chi2;
        int nMult1;
        int nMult2;
        int phiPatHits;
        int patHits;
        int stationCode;
        std::string station;
        double invcurvature;
        double patPhi;
        double patTheta;
        double patMomentum;
        const MuonPatternCombination* patPoint;
        std::vector<const MuonPatternCombination*> patPointers;

        std::ostream& print(std::ostream& os) const;
        bool contains(const MuonPatternCombination* pat) const;
    };

}  // namespace Muon

std::ostream& operator<<(std::ostream& os, const Muon::MCSCSegmentInfo& info);

#endif
