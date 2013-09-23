/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMakerUtils/MuonSegmentCombiSummary.h"


namespace Muon {

  MuonSegmentCombiSummary::MuonSegmentCombiSummary( const MuonSegmentCombination& combi ) : 
    m_segmentsPerChamberLayer(MuonStationIndex::ChIndexMax), m_combi(&combi), combiOriginal(0), nsegments(0)
  {
    
  }


  MuonSegmentCombiSummary::~MuonSegmentCombiSummary() {

  }

}
