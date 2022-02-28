/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRECTOOLINTERFACES_HOUGHDATAPERSEC_H
#define MUONRECTOOLINTERFACES_HOUGHDATAPERSEC_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "AthenaKernel/CLASS_DEF.h"
#include "MuonClusterization/TgcHitClustering.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonPhiLayerHough.h"
#include "MuonLayerHough/MuonRegionHough.h"

namespace Muon {
    struct HoughDataPerSec {
        using HitVec =  MuonHough::HitVec;
        using PhiHitVec = MuonHough::PhiHitVec;
        
        using MaximumVec = std::vector<std::shared_ptr<MuonHough::MuonLayerHough::Maximum>>;
        using PhiMaximumVec =  std::vector<std::shared_ptr<MuonHough::MuonPhiLayerHough::Maximum>>;
        
        typedef std::vector<HitVec> RegionHitVec;
        typedef std::vector<PhiHitVec> RegionPhiHitVec;
        typedef std::map<MuonHough::MuonLayerHough::Maximum*, MaximumVec> MaximumAssociationMap;
        typedef std::vector<MaximumVec> RegionMaximumVec;
        typedef std::vector<PhiMaximumVec> RegionPhiMaximumVec;

        HoughDataPerSec() {
            sector = -1;
            hitVec.resize(MuonStationIndex::sectorLayerHashMax());
            maxVec.resize(MuonStationIndex::sectorLayerHashMax());
            phiHitVec.resize(MuonStationIndex::DetectorRegionIndexMax);
            phiMaxVec.resize(MuonStationIndex::DetectorRegionIndexMax);
            nlayersWithMaxima.resize(MuonStationIndex::DetectorRegionIndexMax);
            nphilayersWithMaxima.resize(MuonStationIndex::DetectorRegionIndexMax);
            nmaxHitsInRegion.resize(MuonStationIndex::DetectorRegionIndexMax);
            nphimaxHitsInRegion.resize(MuonStationIndex::DetectorRegionIndexMax);
        }

        ~HoughDataPerSec() = default;
        

        int sector;
        RegionHitVec hitVec;            // Owns the contained objects
        RegionPhiHitVec phiHitVec;      // Owns the contained objects
        RegionMaximumVec maxVec;        // Owns the contained objects
        RegionPhiMaximumVec phiMaxVec;  // Owns the contained objects
        std::vector<int> nlayersWithMaxima;
        std::vector<int> nphilayersWithMaxima;
        std::vector<int> nmaxHitsInRegion;
        std::vector<int> nphimaxHitsInRegion;
        MaximumAssociationMap maxAssociationMap;  // stores association of a given maximium with other maxima in neighbouring sectors
        std::set<MuonHough::MuonLayerHough::Maximum*>
            associatedToOtherSector;  // used to flagged maxima that were already associated to another sector
        // returns the number of phi and eta hits in the region with most eta hits
        // regions with phi hits are always prefered over regions without
        int maxEtaHits() const { return std::max(nmaxHitsInRegion[0], std::max(nmaxHitsInRegion[1], nmaxHitsInRegion[2])); }
    };

    struct HoughDataPerSectorVec {
        std::vector<HoughDataPerSec> vec;
        MuonHough::MuonDetectorHough detectorHoughTransforms;  // Kept with the vec because it has references to these objects
        std::vector<std::unique_ptr<TgcHitClusteringObj>>
            tgcClusteringObjs;  // Kept with the vec because it has references to these objects
    };
}  // namespace Muon

CLASS_DEF(Muon::HoughDataPerSec, 163257499, 1)
CLASS_DEF(Muon::HoughDataPerSectorVec, 61014906, 1)

#endif
