/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/SETrackInfo.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>

namespace Analysis {

SETrackInfo::SETrackInfo() : m_electron(ElementLink<ElectronContainer>()),
			     m_photon(ElementLink<PhotonContainer>()),
                             m_valD0wrtPV(0),
                             m_pTrel(0)
{
}

SETrackInfo::SETrackInfo(const ElectronContainer* coll, 
			 const Electron* mu,
			 double d0val,
			 double pTrel,
			 const std::vector<double>& tagLikelihood) {
  ElementLink<ElectronContainer> link;
  if( link.toContainedElement(*coll, const_cast<Electron*>(mu)) ) {
    m_electron = link;
  }
  m_valD0wrtPV    = d0val;
  m_pTrel         = pTrel;
  m_tagLikelihood = tagLikelihood;
}

SETrackInfo::SETrackInfo(const PhotonContainer* coll, 
			 const Photon* mu,
			 double d0val,
			 double pTrel,
			 const std::vector<double>& tagLikelihood) {
  ElementLink<PhotonContainer> link;
  if( link.toContainedElement(*coll, const_cast<Photon*>(mu)) ) {
    m_photon = link;
  }
  m_valD0wrtPV    = d0val;
  m_pTrel         = pTrel;
  m_tagLikelihood = tagLikelihood;
}
	
SETrackInfo::SETrackInfo(const ElementLink<ElectronContainer>& eleLink,
			 double d0val,
			 double pTrel,
			 std::vector<double>&& tagLikelihood)
  : m_electron (eleLink),
    m_valD0wrtPV (d0val),
    m_pTrel (pTrel),
    m_tagLikelihood (std::move (tagLikelihood))
 {
}

SETrackInfo::SETrackInfo(const ElementLink<PhotonContainer>& gamLink,
			 double d0val,
			 double pTrel,
			 std::vector<double>&& tagLikelihood)
  : m_photon (gamLink),
    m_valD0wrtPV (d0val),
    m_pTrel (pTrel),
    m_tagLikelihood (std::move (tagLikelihood))
 {
}

const egamma* SETrackInfo::electron() const {
  if( m_electron.isValid() ) {
    return *(m_electron);
  } else if ( m_photon.isValid() ) {
    return *(m_photon);
  } else {
    return 0;
  }
}

bool SETrackInfo::isPhoton() const {
  if ( m_photon.isValid() ) return true;
  return false;
}

MsgStream& operator<<( MsgStream& out, const SETrackInfo& info) {
  const egamma* tp = info.electron();
  double phi = 0.;
  double ipt = 0.;
  double cth = 0.;
  if(tp) {
    phi = tp->phi();
    ipt = tp->iPt();
    cth = tp->cotTh();
  }
  out << " -> SETrackInfo " 
      << " isPhoton = " << info.isPhoton()
      << " d0wrtPV= " << info.d0Value() 
      << " original 1/pt,phi,cotTh = "<<ipt<<" "<<phi<<" "<<cth
      << " pTrel= " << info.pTrel()
      << endmsg; 
  return out;
}
  
std::ostream& operator<<( std::ostream& out, const SETrackInfo& info) {
  const egamma* tp = info.electron();
  double phi = 0.;
  double ipt = 0.;
  double cth = 0.;
  if(tp) {
    phi = tp->phi();
    ipt = tp->iPt();
    cth = tp->cotTh();
  }
  out << " -> SETrackInfo " 
      << " isPhoton = " << info.isPhoton()
      << " d0wrtPV= " << info.d0Value() 
      << " original 1/pt,phi,cotTh = "<<ipt<<" "<<phi<<" "<<cth
      << " pTrel= " << info.pTrel()
      << std::endl; 
  return out;
}

} 
