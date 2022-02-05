/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBLAYERMAPPING_H
#define MUONCALIBLAYERMAPPING_H

#include <map>
#include <set>
#include <vector>

#include "MuonCalibExtraTreeEvent/MuonCalibHit_E.h"
#include "MuonCalibExtraUtils/MuonCalibSimpleHoleSearch.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

namespace MuonCalib {

    /** hit information per station layer index */
    using CalibHitE_Ptr = std::shared_ptr<const MuonCalibHit_E>;
    using StationIndexHitsMap = std::map<MuonFixedIdManipulator::StationIndex, std::vector<CalibHitE_Ptr> >;

    /** hit information per station */
    using IdHitsMap = std::map<MuonFixedId, std::vector<CalibHitE_Ptr> >;

    /** hit information per identifier */
    using IdHitMap = std::map<MuonFixedId, CalibHitE_Ptr>;

    /** intersected layers in the simplified geometry per station */
    using StationIntersectedLayerMap = std::map<MuonFixedId, MuonCalibSimpleHoleSearch::Result>;

    /** set with station layer index */
    using StationIndexSet = std::set<MuonFixedIdManipulator::StationIndex>;

    /** set of identifiers */
    using IdSet = std::set<MuonFixedId>;

}  // namespace MuonCalib

#endif
