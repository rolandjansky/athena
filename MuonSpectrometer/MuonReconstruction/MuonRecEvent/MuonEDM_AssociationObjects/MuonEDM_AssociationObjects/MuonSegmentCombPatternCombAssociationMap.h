/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_EDM_ASSOCIATIONOBJECTS_MUONSEGPATCOMBASSOCMAP_H
#define MUON_EDM_ASSOCIATIONOBJECTS_MUONSEGPATCOMBASSOCMAP_H
#include "CLIDSvc/CLASS_DEF.h"
#include <map>

namespace Muon {
	class MuonSegmentCombination;
	class MuonPatternCombination;
	typedef std::multimap<const MuonSegmentCombination*, const MuonPatternCombination*> MuonSegmentCombPatternCombAssociationMap;
}

CLASS_DEF( Muon::MuonSegmentCombPatternCombAssociationMap , 22914797 , 1 )


#endif