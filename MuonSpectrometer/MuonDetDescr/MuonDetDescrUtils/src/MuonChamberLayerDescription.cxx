/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonDetDescrUtils/MuonChamberLayerDescription.h"

#include <iostream>

namespace Muon {

    MuonChamberLayerDescription::MuonChamberLayerDescription() { initDefaultRegions(); }

    MuonChamberLayerDescriptor MuonChamberLayerDescription::getDescriptor(int sector, Muon::MuonStationIndex::DetectorRegionIndex region,
                                                                          Muon::MuonStationIndex::LayerIndex layer) const {
        bool isSmall = (sector % 2 == 0);
        Muon::MuonStationIndex::ChIndex chIndex = Muon::MuonStationIndex::toChamberIndex(region, layer, isSmall);
        if (chIndex < 0 || chIndex >= Muon::MuonStationIndex::ChIndexMax) {
            MuonChamberLayerDescriptor descriptor;
            return descriptor;
        }

        MuonChamberLayerDescriptor descriptor = m_chamberLayerDescriptors[chIndex];
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

    void MuonChamberLayerDescription::initDefaultRegions() {
        m_chamberLayerDescriptors.resize(Muon::MuonStationIndex::CSS);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::BIS] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BIS, 4560, -7500, 7500, 30, 0.1, 3);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::BIL] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BIL, 4950, -7000, 7000, 30, 0.1, 3);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::BMS] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BMS, 8096, -9500, 9500, 30, 0.1, 5);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::BML] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BML, 7153, -9500, 9500, 30, 0.1, 5);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::BOS] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BOS, 10570, -13500, 13500, 30, 0.1, 7);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::BOL] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::Barrel, Muon::MuonStationIndex::BOL, 9500, -13500, 13500, 30, 0.1, 7);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::BEE] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::BEE, 4415, 7500, 13000, 30, 0.1, 5);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::EIS] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EIS, 7270, 1000, 7000, 30, .05, 3);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::EIL] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EIL, 7675, 1000, 8000, 30, .05, 3);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::EES] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EES, 10800, 4000, 10000, 30, 0.1, 5);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::EEL] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EEL, 11330, 4000, 10000, 30, 0.1, 5);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::EMS] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EMS, 13872, 1500, 13000, 30, 0.1, 5);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::EML] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EML, 14310, 1500, 13000, 30, 0.1, 5);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::EOS] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EOS, 21841, 2000, 13500, 30, 0.1, 7);
        m_chamberLayerDescriptors[Muon::MuonStationIndex::EOL] =
            MuonChamberLayerDescriptor(1, Muon::MuonStationIndex::EndcapA, Muon::MuonStationIndex::EOL, 21421, 2000, 13500, 30, 0.1, 7);
    }

}  // namespace Muon
