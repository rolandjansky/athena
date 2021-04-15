/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONLAYERSURFACE_H
#define MUON_MUONLAYERSURFACE_H

#include "MuonStationIndex/MuonStationIndex.h"
#include "TrkSurfaces/Surface.h"
#include "memory"

namespace Muon {

    /** types */
    struct MuonLayerSurface {
        typedef std::shared_ptr<const Trk::Surface> SurfacePtr;

        MuonLayerSurface() :
            surfacePtr(nullptr),
            sector(-1),
            regionIndex(MuonStationIndex::DetectorRegionUnknown),
            layerIndex(MuonStationIndex::LayerUnknown) {}

        MuonLayerSurface(SurfacePtr surfacePtr_, int sector_, MuonStationIndex::DetectorRegionIndex regionIndex_,
                         MuonStationIndex::LayerIndex layerIndex_) :
            surfacePtr(surfacePtr_), sector(sector_), regionIndex(regionIndex_), layerIndex(layerIndex_) {}

        SurfacePtr surfacePtr;
        int sector;
        MuonStationIndex::DetectorRegionIndex regionIndex;
        MuonStationIndex::LayerIndex layerIndex;
    };

}  // namespace Muon

#endif
