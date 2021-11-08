/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/MuonRegionHough.h"

namespace MuonHough {

    MuonSectorHough::MuonSectorHough(int sector, const MuonDetectorDescription& regionDescriptions) {
        m_transforms.resize(Muon::MuonStationIndex::sectorLayerHashMax(), nullptr);

        // loop over regions and layers of the detector and build the transforms
        for (int reg = 0; reg < Muon::MuonStationIndex::DetectorRegionIndexMax; ++reg) {
            Muon::MuonStationIndex::DetectorRegionIndex region = static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(reg);
            for (int lay = 0; lay < Muon::MuonStationIndex::LayerIndexMax; ++lay) {
                Muon::MuonStationIndex::LayerIndex layer = static_cast<Muon::MuonStationIndex::LayerIndex>(lay);

                // skip the few empty slots in the hash
                RegionDescriptor descriptor = regionDescriptions.getDescriptor(sector, region, layer);
                if (descriptor.chIndex == Muon::MuonStationIndex::ChUnknown) continue;

                int index = Muon::MuonStationIndex::sectorLayerHash(region, layer);
                // std::cout << " creating transform: sector " << sector << " " << Muon::MuonStationIndex::regionName(region)
                //           << " " << Muon::MuonStationIndex::layerName(layer)
                //           << " " << Muon::MuonStationIndex::stName(Muon::MuonStationIndex::toStationIndex(region,layer))
                //           << " " << Muon::MuonStationIndex::chName(descriptor.chIndex)
                //           << " index " << index << " max " << m_transforms.size() << std::endl;
                // if( index >= m_transforms.size() ) std::cout << " index out of bound creating hough  " <<
                // Muon::MuonStationIndex::regionName(region)
                //                                              << " " << Muon::MuonStationIndex::layerName(layer) << std::endl;
                m_transforms[index] = new MuonLayerHough(descriptor);
            }
        }
    }

    MuonSectorHough::~MuonSectorHough() {
        for (auto& transform : m_transforms) delete transform;
    }

    void MuonSectorHough::reset() {
        for (auto& transform : m_transforms) {
            if (transform) transform->reset();
        }
    }

    void MuonDetectorHough::reset() {
        for (auto& sector : m_sectors) sector->reset();
        for (auto& transform : m_phiTransforms) transform->reset();
    }

    MuonDetectorHough::MuonDetectorHough() { init(); }

    /// destructor
    MuonDetectorHough::~MuonDetectorHough() {
        for (auto& sector : m_sectors) delete sector;
        for (auto& transform : m_phiTransforms) delete transform;
    }

    void MuonDetectorHough::init() {
        MuonDetectorDescription detectorDescription;  // initialize all the regions
        for (unsigned int i = 1; i <= 16; ++i) { m_sectors.push_back(new MuonSectorHough(i, detectorDescription)); }
        for (unsigned int i = 0; i < Muon::MuonStationIndex::DetectorRegionIndexMax; ++i) {
            m_phiTransforms.push_back(new MuonPhiLayerHough(60, -M_PI, M_PI, static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(i)));
        }
    }

    MuonDetectorDescription::MuonDetectorDescription() { initDefaultRegions(); }

    RegionDescriptor MuonDetectorDescription::getDescriptor(int sector, Muon::MuonStationIndex::DetectorRegionIndex region,
                                                            Muon::MuonStationIndex::LayerIndex layer) const {
        bool isSmall = (sector % 2 == 0);
        Muon::MuonStationIndex::ChIndex chIndex = Muon::MuonStationIndex::toChamberIndex(region, layer, isSmall);
        // std::cout << "chamberIndex " << chIndex << " region " << region << " layer " << layer << " isSmall " << isSmall << std::endl ;
        // if (region%2 != 1 && layer == 4 && !isSmall){//fix bee chamber in the endcap, add them back in for larger chambers//possible bug
        // in chamber to index
        //   chIndex = Muon::MuonStationIndex::BEE;
        // }

        if (chIndex < 0 || chIndex >= Muon::MuonStationIndex::ChIndexMax) {
            RegionDescriptor descriptor;
            return descriptor;
        }

        RegionDescriptor descriptor = m_regionDescriptions[chIndex];
        descriptor.sector = sector;
        // exceptions for a few barrel regions
        if (region == Muon::MuonStationIndex::Barrel) {
            if ((sector == 10 || sector == 14) && layer == Muon::MuonStationIndex::Inner)
                descriptor.referencePosition = 5400.;
            else if ((sector == 11 || sector == 13) && layer == Muon::MuonStationIndex::Outer)
                descriptor.referencePosition = 10650.;
        } else if (region == Muon::MuonStationIndex::EndcapC) {  // multiply reference position by -1 for C side
            descriptor.region = region;
            if (layer == Muon::MuonStationIndex::BarrelExtended) {
                descriptor.yMinRange *= -1;
                descriptor.yMaxRange *= -1;
                std::swap(descriptor.yMinRange, descriptor.yMaxRange);
            } else {
                descriptor.referencePosition *= -1;
            }
        }

        if (descriptor.chIndex < 0 || descriptor.chIndex >= Muon::MuonStationIndex::ChIndexMax) {
            std::cout << " bad descriptor!!!! " << Muon::MuonStationIndex::regionName(region) << " "
                      << Muon::MuonStationIndex::layerName(layer) << " " << isSmall << " " << chIndex << std::endl;
        }
        return descriptor;
    }

    void MuonDetectorDescription::initDefaultRegions() {
        double scalefactor = 1.0;               // can be used to tune the steps in theta variation!
        int inner_step = 3;                     // default is 3
        int middle_step = 5 * scalefactor;      // default is 5--range is 0.25
        int outer_step = 7 * scalefactor;       // default is 7--range is 0.35
        double inner_gap = 0.05;                // default is 0.05
        double middle_gap = 0.1 / scalefactor;  // default is 0.1
        double outer_gap = 0.1 / scalefactor;   // default is 0.1
        int ystep = 30;                         // default is 30
        m_regionDescriptions.resize(Muon::MuonStationIndex::CSS);
        m_regionDescriptions[Muon::MuonStationIndex::BIS] = RegionDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BIS,
                                                                             4560, -7500, 7500, ystep, middle_gap, inner_step);
        m_regionDescriptions[Muon::MuonStationIndex::BIL] = RegionDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BIL,
                                                                             4950, -7000, 7000, ystep, middle_gap, inner_step);
        m_regionDescriptions[Muon::MuonStationIndex::BMS] = RegionDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BMS,
                                                                             8096, -9500, 9500, ystep, middle_gap, middle_step);
        m_regionDescriptions[Muon::MuonStationIndex::BML] = RegionDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BML,
                                                                             7153, -9500, 9500, ystep, middle_gap, middle_step);
        m_regionDescriptions[Muon::MuonStationIndex::BOS] = RegionDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BOS,
                                                                             10570, -13500, 13500, ystep, outer_gap, outer_step);
        m_regionDescriptions[Muon::MuonStationIndex::BOL] = RegionDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BOL,
                                                                             9500, -13500, 13500, ystep, outer_gap, outer_step);
        m_regionDescriptions[Muon::MuonStationIndex::BEE] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::BEE, 4415, 7500, 13000, ystep, middle_gap, middle_step);
        m_regionDescriptions[Muon::MuonStationIndex::EIS] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EIS, 7270, 1000, 7000, ystep, inner_gap, inner_step);  // 7
        m_regionDescriptions[Muon::MuonStationIndex::EIL] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EIL, 7675, 1000, 8000, ystep, inner_gap, inner_step);
        m_regionDescriptions[Muon::MuonStationIndex::EES] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EES, 10800, 4000, 10000, ystep, middle_gap, middle_step);
        m_regionDescriptions[Muon::MuonStationIndex::EEL] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EEL, 11330, 4000, 10000, ystep, middle_gap, middle_step);
        m_regionDescriptions[Muon::MuonStationIndex::EMS] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EMS, 13872, 1500, 13000, ystep, middle_gap, middle_step);
        m_regionDescriptions[Muon::MuonStationIndex::EML] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EML, 14310, 1500, 13000, ystep, middle_gap, middle_step);
        m_regionDescriptions[Muon::MuonStationIndex::EOS] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EOS, 21841, 2000, 13500, ystep, outer_gap, outer_step);
        m_regionDescriptions[Muon::MuonStationIndex::EOL] = RegionDescriptor(
            1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EOL, 21421, 2000, 13500, ystep, outer_gap, outer_step);
    }

}  // namespace MuonHough
