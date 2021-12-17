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

  StacoTag::~StacoTag() = default;

  std::string StacoTag::name() const { return "StacoTag"; }
  bool StacoTag::isComissioning() const { return muonCandidate().isComissioning(); }
  std::string StacoTag::toString() const { 
    std::ostringstream sout;
    sout << name() << " match chi2 " << matchChi2(); 
    return sout.str();
  }
  const Trk::Perigee& StacoTag::combinedParameters() const { return *m_combinedParameters.get(); }
  std::unique_ptr<const Trk::Perigee> StacoTag::releaseCombinedParameters() { return std::move(m_combinedParameters); }
  const MuonCandidate& StacoTag::muonCandidate() const { return *m_muonCandidate; }
  double StacoTag::matchChi2() const { return m_chi2; }
  const Trk::Track* StacoTag::primaryTrack() const { return muonCandidate().primaryTrack();}

  bool operator<( const StacoTag& t1,  const StacoTag& t2 ){
    return t1.matchChi2() < t2.matchChi2();
  }


}
