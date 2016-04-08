/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegment/MuonSegmentQuality.h"



namespace Muon {


  MuonSegmentQuality::MuonSegmentQuality() : FitQuality(), m_isStrict(false) {}

  MuonSegmentQuality::MuonSegmentQuality( double chi2, int ndof, 
					  const std::vector<Identifier>& channelsWithoutHit, bool isStrict) 
:
    FitQuality(chi2, ndof), m_channelsWithoutHit(channelsWithoutHit), m_isStrict(isStrict)
  {
     
  }

  MuonSegmentQuality::MuonSegmentQuality( double chi2, int ndof, 
					  std::vector<Identifier>&& channelsWithoutHit, bool isStrict) 
:
    FitQuality(chi2, ndof), m_channelsWithoutHit(std::move(channelsWithoutHit)), m_isStrict(isStrict)
  {
     
  }


}
