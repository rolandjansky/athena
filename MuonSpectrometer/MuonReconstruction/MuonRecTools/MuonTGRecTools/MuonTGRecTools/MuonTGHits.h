/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTGHits.h, (c) ATLAS Detector Softwareop 
///////////////////////////////////////////////////////////////////

#ifndef MUONTGRECTOOLS_MUONTGHITS_H
#define MUONTGRECTOOLS_MUONTGHITS_H

#include "DataModel/DataVector.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "CLIDSvc/CLASS_DEF.h"

namespace Muon{

typedef std::pair<const Trk::Layer*,std::vector<const Trk::PrepRawData*>*> PairOfLayerPrd;
typedef std::pair<const Trk::DetachedTrackingVolume*,std::vector<PairOfLayerPrd*>*> StationPrd;
typedef DataVector<StationPrd> MuonTGHits;
}
CLASS_DEF(Muon::MuonTGHits,55556,1)

#endif //MUONTGRECTOOLS_MUONTGHITS_H

