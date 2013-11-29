/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENT_MUONSEGMENTCOMBINATIONCOLLECTION_H
#define MUON_MUONSEGMENT_MUONSEGMENTCOMBINATIONCOLLECTION_H

#include "DataModel/DataVector.h"
#include "MuonSegment/MuonSegmentCombination.h"
#include "CLIDSvc/CLASS_DEF.h"

/**This typedef represents a collection of MuonSegmentCombination objects. 
It is a DataVector. It can be saved
to storegate, and written out via pool*/

typedef DataVector<Muon::MuonSegmentCombination> MuonSegmentCombinationCollection;
CLASS_DEF(MuonSegmentCombinationCollection , 1314529658 , 1 )

#endif
