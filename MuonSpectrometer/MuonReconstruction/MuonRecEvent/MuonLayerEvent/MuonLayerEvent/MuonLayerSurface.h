/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERSURFACE_H
#define MUON_MUONLAYERSURFACE_H

#include "MuonStationIndex/MuonStationIndex.h"
#include "TrkSurfaces/Surface.h"
#include "memory"

namespace Muon {

    /** types */
    struct MuonLayerSurface {
        using SurfacePtr = std::shared_ptr<const Trk::Surface>;

        MuonLayerSurface() = default;

        MuonLayerSurface(SurfacePtr surfacePtr_, int sector_, MuonStationIndex::DetectorRegionIndex regionIndex_,
                         MuonStationIndex::LayerIndex layerIndex_) :
            surfacePtr{surfacePtr_}, sector{sector_}, regionIndex{regionIndex_}, layerIndex{layerIndex_} {}

        inline MuonStationIndex::StIndex stIndex() const {
            return Muon::MuonStationIndex::toStationIndex(regionIndex, layerIndex);
        }
        SurfacePtr surfacePtr{nullptr};
        int sector{-1};
        MuonStationIndex::DetectorRegionIndex regionIndex{MuonStationIndex::DetectorRegionUnknown};
        MuonStationIndex::LayerIndex layerIndex{MuonStationIndex::LayerUnknown};
    };

}  // namespace Muon

#endif
