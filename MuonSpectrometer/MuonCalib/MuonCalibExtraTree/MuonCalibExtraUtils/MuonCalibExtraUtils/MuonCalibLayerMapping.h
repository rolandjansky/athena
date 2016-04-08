/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBLAYERMAPPING_H
#define MUONCALIBLAYERMAPPING_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include "MuonCalibExtraUtils/MuonCalibSimpleHoleSearch.h"


#include <vector>
#include <map>
#include <set>

namespace MuonCalib {
  
  class MuonCalibHit_E;

  /** hit information per station layer index */
  typedef std::map< MuonFixedIdManipulator::StationIndex,std::vector<const MuonCalibHit_E*> > StationIndexHitsMap;

  /** hit information per station */
  typedef std::map<MuonFixedId,std::vector<const MuonCalibHit_E*> > IdHitsMap;

  /** hit information per identifier */
  typedef std::map<MuonFixedId,const MuonCalibHit_E*> IdHitMap;

  /** intersected layers in the simplified geometry per station */
  typedef std::map<MuonFixedId,MuonCalibSimpleHoleSearch::Result> StationIntersectedLayerMap;

  /** set with station layer index */
  typedef std::set<MuonFixedIdManipulator::StationIndex> StationIndexSet;

  /** set of identifiers */
  typedef std::set<MuonFixedId> IdSet;
  

}

#endif
