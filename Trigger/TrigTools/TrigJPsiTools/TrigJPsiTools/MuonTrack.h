// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File name: MuonTrack.h
// Description: General muon track class
// Author: Peter Wijeratne (paw@hep.ucl.ac.uk)

#ifndef MUONTRACK_H
#define MUONTRACK_H

#include <iostream>
#include <string>

#include "Particle/TrackParticle.h"

namespace Wrapper{

  class MuonTrack
    {

    public:

      MuonTrack() { ; }
      MuonTrack(double  eta, double  phi, double  z0, double  a0, double  pT, 
		double px, double py, double pz, double charge, double chi2, double chi2prob, 
		double deta, double dphi, double dz0, double da0, double dpT, 
		int nCscHitsEta=0, int nCscHitsPhi=0, int nTgcHitsEta=0, int nTgcHitsPhi=0,
		int nRpcHitsEta=0,  int nRpcHitsPhi=0, int nMdtHits=0,
		unsigned hitPattern=0,
		unsigned multiPattern=0,
		int author=0,
		const Rec::TrackParticle* match=0,
		int roiNum=0) :
	m_eta(eta),   m_phi(phi),   m_z0(z0),   m_a0(a0),   m_pT(pT), 
	m_px(px), m_py(py), m_pz(pz), m_charge(charge), m_chi2(chi2), m_chi2prob(chi2prob),
	m_deta(deta), m_dphi(dphi), m_dz0(dz0), m_da0(da0), m_dpT(dpT),
	m_nCscHitsEta(nCscHitsEta), m_nCscHitsPhi(nCscHitsPhi), m_nTgcHitsEta(nTgcHitsEta),  m_nTgcHitsPhi(nTgcHitsPhi),
	m_nRpcHitsEta(nRpcHitsEta), m_nRpcHitsPhi(nRpcHitsPhi), m_nMdtHits(nMdtHits),
	m_hitPattern(hitPattern),
	m_multiPattern(multiPattern),
	m_author(author),
	m_match(match),
	m_roiNum(roiNum)
      {} 
      
      virtual ~MuonTrack() { ; }

      double eta() const { return m_eta; }
      double phi() const { return m_phi; }
      double z0()  const { return m_z0; }
      double a0()  const { return m_a0; }
      double pt()  const { return m_pT; }
      double px() const { return m_px; }
      double py() const { return m_py; }
      double pz() const { return m_pz; }
      
      double charge() const { return m_charge; }
      
      double chi2() const {return m_chi2;}
      double chi2prob() const {	return m_chi2prob; }
  
      double deta() const {return m_deta;}
      double dphi() const {return m_dphi;}
      double dz0()  const {return m_dz0;}
      double da0()  const {return m_da0;}
      double dpT()  const {return m_dpT;}  

      int nCscHitsPhi()  const  { return m_nCscHitsPhi; }
      int nTgcHitsEta()    const  { return m_nTgcHitsEta; }
      int nTgcHitsPhi()     const  { return m_nTgcHitsPhi; }
      int nCscHitsEta() const  { return m_nCscHitsEta; }
      int nRpcHitsEta()  const {return m_nRpcHitsEta;}
      int nRpcHitsPhi()     const {return m_nRpcHitsPhi;}  
      int nMdtHits() const { return m_nMdtHits; }
      
      unsigned hitPattern()   const { return m_hitPattern; } 
      unsigned multiPattern() const { return m_multiPattern; } 

      int author() const { return m_author; }  

      const Rec::TrackParticle* match() const { return m_match; }

      int roiNum() const { return m_roiNum; }

    public:

      bool operator==(const MuonTrack& t) { 
    
	if( m_author != t.m_author ) return false;
    
	if( m_eta != t.m_eta )  return false;
	if( m_phi != t.m_phi )  return false;
	if( m_z0  != t.m_z0 )   return false;
	if( m_a0  != t.m_a0 )   return false;
	if( m_pT  != t.m_pT )   return false;
  	     
	if( m_nCscHitsEta  != t.m_nCscHitsEta ) return false;
	if( m_nCscHitsPhi   != t.m_nCscHitsPhi )  return false;
	if( m_nTgcHitsEta     != t.m_nTgcHitsEta )    return false;
	if( m_nTgcHitsPhi      != t.m_nTgcHitsPhi )     return false;
	if( m_nRpcHitsEta   != t.m_nRpcHitsEta )  return false;
	if( m_nRpcHitsPhi      != t.m_nRpcHitsPhi )     return false;
	if ( m_hitPattern != t.m_hitPattern ) return false;
	if ( m_multiPattern != t.m_multiPattern ) return false;
    
	return true;
      }

    protected:
  
      // Track parameters
      double m_eta,  m_phi,  m_z0,  m_a0,  m_pT, m_px, m_py, m_pz, m_charge, m_chi2, m_chi2prob;
      double m_deta, m_dphi, m_dz0, m_da0, m_dpT;

      // Track hits
      int m_nCscHitsEta, m_nCscHitsPhi, m_nTgcHitsEta, m_nTgcHitsPhi;
      int m_nRpcHitsEta, m_nRpcHitsPhi;
      int m_nMdtHits;
  
      // hit bit pattern
      unsigned m_hitPattern;
      unsigned m_multiPattern;

      //Track author;
      int m_author;  

      //matched
      const Rec::TrackParticle* m_match;

      //roi number
      int m_roiNum;

    };

}

inline std::ostream& operator<<( std::ostream& s, const Wrapper::MuonTrack& t) { 
  return s<< "[ " 
	  << "\teta=" << t.eta()
	  << "\tphi=" << t.phi()
	  << "\tz0="  << t.z0()
	  << "\tpT="  << t.pt()
	  << "\td0="  << t.a0() 
	  << "\thp=0x"  << std::hex << t.hitPattern() << std::dec << "  "
	  << " ]";
}

#endif
