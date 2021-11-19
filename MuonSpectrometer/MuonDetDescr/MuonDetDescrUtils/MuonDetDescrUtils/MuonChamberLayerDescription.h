/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONCHAMBERLAYERDESCRIPTION_H
#define MUON_MUONCHAMBERLAYERDESCRIPTION_H

#include <vector>

#include "MuonDetDescrUtils/MuonChamberLayerDescriptor.h"

namespace Muon {

    /** class managing geometry of the chamber layers */
    class MuonChamberLayerDescription {
    public:
        /// constructor
        MuonChamberLayerDescription();

        MuonChamberLayerDescriptor getDescriptor(int sector, Muon::MuonStationIndex::DetectorRegionIndex region,
                                                 Muon::MuonStationIndex::LayerIndex layer) const;

    private:
        /// initialize default geometry
        void initDefaultRegions();

        /// cached geometry
        typedef std::vector<MuonChamberLayerDescriptor> MuonChamberLayerDescriptorVec;
        MuonChamberLayerDescriptorVec m_chamberLayerDescriptors;  /// region descriptions
    };
}  // namespace Muon

#endif
