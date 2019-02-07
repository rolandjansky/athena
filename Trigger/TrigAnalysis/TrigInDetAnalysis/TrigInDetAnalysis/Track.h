/* emacs: this is -*- c++ -*- */
/**
 **     @file    Track.h
 **
 **     @author  mark sutton
 **     @date    Thu 10 Jan 2019 19:58:49 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDA_TRACK_H
#define TIDA_TRACK_H

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

#include "TObject.h"

namespace TIDA {
  

class Track  : public TObject {
    
public:
    
  Track();
  Track(double  eta, double  phi, double  z0, double  a0, double  pT, double chi2, double dof,
	double deta, double dphi, double dz0, double da0, double dpT, 
	int bLayerHits=0, int pixelHits=0, int sctHits=0, int siHits=0,
	int strawHits=0,  int trHits=0, 
	unsigned hitPattern=0,
	unsigned multiPattern=0,
	int author=0,
	bool hasTruth=false,
	int barcode = -1,
	int match_barcode = -1,
	bool expectBL=false,
	unsigned long id=0);

  virtual ~Track();

  double eta() const { return m_eta; }
  double phi() const { return m_phi; }
  double z0()  const { return m_z0; }
  double a0()  const { return m_a0; }
  double pT()  const { return m_pT; }

  double chi2() const {return m_chi2;}
  double dof()  const {return m_dof;}
  
  double deta() const {return m_deta;}
  double dphi() const {return m_dphi;}
  double dz0()  const {return m_dz0;}
  double da0()  const {return m_da0;}
  double dpT()  const {return m_dpT;}  

  int  pixelHits()  const  { return m_pixelHits%1000; }
  bool expectBL()   const  { return m_expectBL; }
  int  sctHits()    const  { return m_sctHits%1000; }
  int  siHits()     const  { return m_siHits%1000; }
  int  bLayerHits() const  { return m_bLayerHits%1000; }

  int strawHits()  const {return m_strawHits%1000;}
  int trHits()     const {return m_trHits%1000;}  


  int  pixelHoles()  const  { return m_pixelHits/1000; }
  int  sctHoles()    const  { return m_sctHits/1000; }
  int  siHoles()     const  { return m_siHits/1000; }
  int  bLayerHoles() const  { return m_bLayerHits/1000; }

  int strawHoles()  const {return m_strawHits/1000;}
  int trHoles()     const {return m_trHits/1000;}  


  unsigned hitPattern()   const { return m_hitPattern; } 
  unsigned multiPattern() const { return m_multiPattern; } 

  int author() const { return m_author; }  

  int barcode() const { return m_barcode;}
  int match_barcode() const { return m_match_barcode;}
  
  unsigned long id() const { return m_id; }

  bool hasTruth() const { return m_hasTruth; }  
	

public:

  bool operator==(const Track& t) { 
    
    if( m_author != t.m_author ) return false;
    
    if( m_eta != t.m_eta )  return false;
    if( m_phi != t.m_phi )  return false;
    if( m_z0  != t.m_z0 )   return false;
    if( m_a0  != t.m_a0 )   return false;
    if( m_pT  != t.m_pT )   return false;
  	     
    if( m_bLayerHits  != t.m_bLayerHits ) return false;
    if( m_expectBL    != t.m_expectBL   ) return false;
    if( m_pixelHits   != t.m_pixelHits )  return false;
    if( m_sctHits     != t.m_sctHits )    return false;
    if( m_siHits      != t.m_siHits )     return false;
    if( m_strawHits   != t.m_strawHits )  return false;
    if( m_trHits      != t.m_trHits )     return false;
    if ( m_hitPattern != t.m_hitPattern ) return false;
    if ( m_multiPattern != t.m_multiPattern ) return false;
    if ( m_id != t.m_id ) return false;
    
    return true;
  }

protected:
  
  // Track parameters
  double m_eta,  m_phi,  m_z0,  m_a0,  m_pT,  m_chi2;
  double m_dof; 
  double m_deta, m_dphi, m_dz0, m_da0, m_dpT;

  /// Track hits and holes - encoded as 
  /// 1000*nholes + nhits the relevant %1000 or /1000
  /// should be performed by the relevant accessor to 
  /// return either the number of hits or holes
  int m_bLayerHits, m_pixelHits, m_sctHits, m_siHits;
  int m_strawHits, m_trHits;
  
  // hit bit pattern
  unsigned m_hitPattern;
  unsigned m_multiPattern;

  //Track author;
  int m_author;

  //
  bool m_hasTruth;
  int m_barcode;
  int m_match_barcode;

  //
  bool m_expectBL;
  
  /// identifier
  unsigned long m_id;

  ClassDef(TIDA::Track,6);

};
  
}

 

inline std::string hextobin(const unsigned& h, int nbits=32) { 
  std::string s;
  for ( int i=nbits ; i-- ; ) s += ( (h>>i&0x1) ? "1" : "0" );
  return s;
}

  
inline std::ostream& operator<<( std::ostream& s, const TIDA::Track& t) { 
    return s<< "[ " 
	    << "\teta=" << t.eta()
	    << "\tphi=" << t.phi()
	    << "\tz0="  << t.z0()
      	    << "\tpT="  << t.pT()*0.001 << " GeV  "
      //  << "\tpT="  << t.pT() 
	    << "\td0="  << t.a0() 
	    << "\thp=0x" << std::hex << t.hitPattern() << std::dec << "  "
      // << "\thpb="  << hextobin(t.hitPattern(),20)
	    << "\tchi2=" << t.chi2() << "/" << t.dof()  
	    << "\talgo=" << t.author()
            << "\tbl="   << t.bLayerHits() 
            << ":"  << ( t.expectBL() ? "t" : "f" )
            << ":"  << ( t.hasTruth() ? "t" : "f" )
	    << "\tid=0x" << std::hex << t.id() << std::dec << ":0x" << std::hex << t.barcode() << std::dec
	    << "\t] ";
}


#endif // TIDA_TRACK_H
