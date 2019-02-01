/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/SMTrackInfo.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

namespace Analysis {

SMTrackInfo::SMTrackInfo()
  : m_muon(ElementLink<MuonContainer>()),
    m_valD0wrtPV(0),
    m_pTrel(0)
{
}

SMTrackInfo::SMTrackInfo(const MuonContainer* coll, 
			 const Muon* mu,
			 double d0val,
			 double pTrel,
			 const std::vector<double>& tagLikelihood) {
  ElementLink<MuonContainer> link;
  if( link.toContainedElement(*coll, const_cast<Muon*>(mu)) ) {
    m_muon = link;
  }
  m_valD0wrtPV = d0val;
  m_pTrel = pTrel;
  m_tagLikelihood = tagLikelihood;
}
	
SMTrackInfo::SMTrackInfo(const ElementLink<MuonContainer>& muonLink,
                         double d0val,
                         double pTrel,
                         std::vector<double>&& tagLikelihood)
  : m_muon (muonLink),
    m_valD0wrtPV (d0val),
    m_pTrel (pTrel),
    m_tagLikelihood (std::move (tagLikelihood))
{
}

const Muon* SMTrackInfo::muon() const {
  if( m_muon.isValid() ) {
    return *(m_muon);
  } else {
    return 0;
  }
}

MsgStream& operator<<( MsgStream& out, const SMTrackInfo& info) {
  const Muon* tp = info.muon();
  double phi = 0.;
  double ipt = 0.;
  double cth = 0.;
  if(tp) {
    phi = tp->phi();
    ipt = tp->iPt();
    cth = tp->cotTh();
  }
  out << " -> SMTrackInfo " 
      << " d0wrtPV= " << info.d0Value() 
      << " original 1/pt,phi,cotTh = "<<ipt<<" "<<phi<<" "<<cth
      << " pTrel= " << info.pTrel()
      << endmsg; 
  return out;
}
  
std::ostream& operator<<( std::ostream& out, const SMTrackInfo& info) {
  const Muon* tp = info.muon();
  double phi = 0.;
  double ipt = 0.;
  double cth = 0.;
  if(tp) {
    phi = tp->phi();
    ipt = tp->iPt();
    cth = tp->cotTh();
  }
  out << " -> SMTrackInfo " 
      << " d0wrtPV= " << info.d0Value() 
      << " original 1/pt,phi,cotTh = "<<ipt<<" "<<phi<<" "<<cth
      << " pTrel= " << info.pTrel()
      << std::endl; 
  return out;
}
  
} 
