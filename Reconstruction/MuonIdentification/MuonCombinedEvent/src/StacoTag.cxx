/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/StacoTag.h"
#include <iostream>

namespace MuonCombined {

  StacoTag::StacoTag( const MuonCandidate& muonCandidate, std::unique_ptr<const Trk::Perigee>& combinedParameters, double chi2 ) : 
    TagBase(TagBase::Author::STACO,TagBase::Type::Combined), 
    m_muonCandidate(&muonCandidate), 
    m_combinedParameters(std::move(combinedParameters)),
    m_chi2(chi2){}

  StacoTag::~StacoTag() {
  }

  std::string StacoTag::toString() const { 
    std::ostringstream sout;
    sout << name() << " match chi2 " << matchChi2(); 
    return sout.str();
  }


}
