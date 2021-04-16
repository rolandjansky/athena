/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTGHits.h, (c) ATLAS Detector Softwareop
///////////////////////////////////////////////////////////////////

#ifndef MUONTGRECTOOLS_MUONTGHITS_H
#define MUONTGRECTOOLS_MUONTGHITS_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkPrepRawData/PrepRawData.h"

namespace Muon {

    typedef std::pair<const Trk::Layer*, std::vector<const Trk::PrepRawData*>*> PairOfLayerPrd;
    typedef std::pair<const Trk::DetachedTrackingVolume*, std::vector<PairOfLayerPrd*>*> StationPrd;
    typedef DataVector<StationPrd> MuonTGHits;
}  // namespace Muon
CLASS_DEF(Muon::MuonTGHits, 55556, 1)

#endif  // MUONTGRECTOOLS_MUONTGHITS_H
