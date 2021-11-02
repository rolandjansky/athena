/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCHAMBERLAYERDESCRIPTOR_H
#define MUON_MUONCHAMBERLAYERDESCRIPTOR_H

#include "MuonStationIndex/MuonStationIndex.h"

namespace Muon {

    /** struct containing all information to build a Hough transform for a given chamber index */
    struct MuonChamberLayerDescriptor {
        MuonChamberLayerDescriptor(int sector_, Muon::MuonStationIndex::DetectorRegionIndex region_,
                                   Muon::MuonStationIndex::ChIndex chIndex_, float referencePosition_, float yMinRange_, float yMaxRange_,
                                   float yBinSize_, float thetaStep_, unsigned int nthetaSamples_) :
            sector(sector_),
            region(region_),
            chIndex(chIndex_),
            referencePosition(referencePosition_),
            yMinRange(yMinRange_),
            yMaxRange(yMaxRange_),
            yBinSize(yBinSize_),
            thetaStep(thetaStep_),
            nthetaSamples(nthetaSamples_) {}
        MuonChamberLayerDescriptor() :
            sector(0),
            region(Muon::MuonStationIndex::DetectorRegionUnknown),
            chIndex(Muon::MuonStationIndex::ChUnknown),
            referencePosition(0),
            yMinRange(0),
            yMaxRange(0),
            yBinSize(1),
            thetaStep(1),
            nthetaSamples(1) {}

        int sector;
        Muon::MuonStationIndex::DetectorRegionIndex region;
        Muon::MuonStationIndex::ChIndex chIndex;
        float referencePosition;
        float yMinRange;
        float yMaxRange;
        float yBinSize;
        float thetaStep;
        unsigned int nthetaSamples;
    };

}  // namespace Muon

#endif
