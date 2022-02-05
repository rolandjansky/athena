/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONREGIONHOUGH_H
#define MUONREGIONHOUGH_H

#include <cmath>
#include <iostream>
#include <vector>

#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonPhiLayerHough.h"
#include "MuonStationIndex/MuonStationIndex.h"

namespace MuonHough {

    /** class managing geometry of the Hough spaces */
    class MuonDetectorDescription {
    public:
        /// constructor
        MuonDetectorDescription();

        RegionDescriptor getDescriptor(int sector, Muon::MuonStationIndex::DetectorRegionIndex region,
                                       Muon::MuonStationIndex::LayerIndex layer) const;

    private:
        /// initialize default geometry
        void initDefaultRegions();

        /// cached geometry
        RegionDescriptionVec m_regionDescriptions;  /// region descriptions
    };

    /** class managing all precision Hough transforms in a sector */
    class MuonSectorHough {
    public:
        /// constructor for a given sector using the default geometry
        MuonSectorHough(int sector, const MuonDetectorDescription& regionDescriptions);

        /// destructor
        ~MuonSectorHough();

        /// access the Hough transform for a given region
        MuonLayerHough& hough(Muon::MuonStationIndex::DetectorRegionIndex region, Muon::MuonStationIndex::LayerIndex layer);

        /// reset histograms
        void reset();

    private:
        std::vector<MuonLayerHough*> m_transforms;  /// Hough transforms for all regions
                                                    // int m_sector;                                /// sector number
    };

    /** class managing all Hough transforms in the detector */
    class MuonDetectorHough {
    public:
        /// access phi transform
        MuonPhiLayerHough& phiHough(Muon::MuonStationIndex::DetectorRegionIndex region);

        /// access precision transform
        MuonLayerHough& hough(int sector, Muon::MuonStationIndex::DetectorRegionIndex region, Muon::MuonStationIndex::LayerIndex layer);

        /// reset histograms
        void reset();

        /// constructor using default region definitions
        MuonDetectorHough();

        /// constructor using custom region definitions
        MuonDetectorHough(const RegionDescriptionVec& regionDescriptors);

        /// destructor
        ~MuonDetectorHough();

    private:
        void init();

        std::vector<MuonSectorHough*> m_sectors;          /// sector transforms
        std::vector<MuonPhiLayerHough*> m_phiTransforms;  /// phi transforms
    };

    inline MuonLayerHough& MuonSectorHough::hough(Muon::MuonStationIndex::DetectorRegionIndex region,
                                                  Muon::MuonStationIndex::LayerIndex layer) {
        int index = Muon::MuonStationIndex::sectorLayerHash(region, layer);
        return *m_transforms[index];
    }

    inline MuonPhiLayerHough& MuonDetectorHough::phiHough(Muon::MuonStationIndex::DetectorRegionIndex region) {
        return *m_phiTransforms[region];
    }

    inline MuonLayerHough& MuonDetectorHough::hough(int sector, Muon::MuonStationIndex::DetectorRegionIndex region,
                                                    Muon::MuonStationIndex::LayerIndex layer) {
        return m_sectors[sector - 1]->hough(region, layer);
    }

}  // namespace MuonHough
#endif
