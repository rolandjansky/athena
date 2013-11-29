/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENT_MUONSEGMENTDICT_H
#define MUON_MUONSEGMENT_MUONSEGMENTDICT_H
#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/T0MuonSegment.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include <vector>

class MuonSegmentDict_Dummy {
    MuonSegmentCombinationCollection        t0;
    std::vector<const Muon::MuonSegment*>   t1;
    std::vector<const Muon::T0MuonSegment*> t2;

};

#endif 
