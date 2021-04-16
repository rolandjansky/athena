/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTGSegments.h, (c) ATLAS Detector Softwareop
///////////////////////////////////////////////////////////////////

#ifndef MUONTGMEASASSOCALG_MUONTGSEGMENTS_H
#define MUONTGMEASASSOCALG_MUONTGSEGMENTS_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkSegment/Segment.h"

namespace Muon {

    typedef std::pair<const Trk::DetachedTrackingVolume*, std::vector<const Trk::Segment*>*> StationSgs;
    typedef DataVector<StationSgs> MuonTGSegments;
}  // namespace Muon
CLASS_DEF(Muon::MuonTGSegments, 55557, 1)

#endif  // MUONTGMEASASSOCALG_MUONTGSEGMENTS_H
