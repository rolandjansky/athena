/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNTOOLS_HOUGHDATAPERSEC_H
#define MUONHOUGHPATTERNTOOLS_HOUGHDATAPERSEC_H

#include "AthenaKernel/CLASS_DEF.h"
#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonPhiLayerHough.h"
#include "MuonLayerHough/MuonRegionHough.h"
#include <map>
#include <set>
#include <vector>


namespace Muon {
  struct HoughDataPerSec {
    typedef std::vector<MuonHough::Hit*>    HitVec;
    typedef std::vector< HitVec >           RegionHitVec;
    typedef std::vector<MuonHough::PhiHit*> PhiHitVec;
    typedef std::vector< PhiHitVec >        RegionPhiHitVec;
    typedef std::vector<MuonHough::MuonLayerHough::Maximum*>    MaximumVec;
    typedef std::vector<MuonHough::MuonPhiLayerHough::Maximum*> PhiMaximumVec; 
    typedef std::map<MuonHough::MuonLayerHough::Maximum*, MaximumVec > MaximumAssociationMap;
    typedef std::vector< MaximumVec >       RegionMaximumVec;
    typedef std::vector< PhiMaximumVec >    RegionPhiMaximumVec;

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
    void cleanUp();
    int                   sector;
    RegionHitVec          hitVec;
    RegionPhiHitVec       phiHitVec;
    RegionMaximumVec      maxVec;
    RegionPhiMaximumVec   phiMaxVec;
    std::vector<int>      nlayersWithMaxima;
    std::vector<int>      nphilayersWithMaxima;
    std::vector<int>      nmaxHitsInRegion;
    std::vector<int>      nphimaxHitsInRegion;
    MaximumAssociationMap maxAssociationMap; // stores association of a given maximium with other maxima in neighbouring sectors
    std::set<MuonHough::MuonLayerHough::Maximum*> associatedToOtherSector; // used to flagged maxima that were already associated to another sector
    // returns the number of phi and eta hits in the region with most eta hits
    // regions with phi hits are always prefered over regions without
    int maxEtaHits() const{
      return std::max( nmaxHitsInRegion[0], std::max( nmaxHitsInRegion[1], nmaxHitsInRegion[2] ) );
    }
  };

  struct HoughDataPerSectorVec : public std::vector<HoughDataPerSec>
  {
    MuonHough::MuonDetectorHough detectorHoughTransforms;
  };
}

CLASS_DEF(Muon::HoughDataPerSec, 163257499, 1)
CLASS_DEF(Muon::HoughDataPerSectorVec, 61014906, 1)

#endif
