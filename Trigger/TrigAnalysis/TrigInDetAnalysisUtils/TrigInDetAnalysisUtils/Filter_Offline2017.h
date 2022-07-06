/* emacs: this is -*- c++ -*- */
/**
 **     @file    Filter_Offline2017.h
 **
 **       Filter class to implement the 2017 CP recomendations
 **       on hadronic tracks as found in 
 **       https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TrackingCPPreRecsSummer2017
 **
 **     @author  mark sutton
 **
 **     Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TRIGINDETANALYSISUTILS_FILTER_OFFLINE2017_H 
#define TRIGINDETANALYSISUTILS_FILTER_OFFLINE2017_H 

#include <iostream>
#include <cmath>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"


// generic selection cut selection function 

class Filter_Offline2017 : public TrackFilter { 

public:
  
  Filter_Offline2017(  double pTMin=1000, const std::string& type="tight", double z0=1000, double a0=1000 ) :  
    m_pTMin(pTMin),
    m_z0max(z0),    
    m_a0max(a0),
    m_type(type)
  {  } 

  bool select(const TIDA::Track* t, const TIDARoiDescriptor* =0 ) { 
    // Select track parameters
    bool selected = true;
    
    int Nsi      = t->pixelHits()*0.5 + t->sctHits();
    int Nsiholes = t->pixelHoles()+t->sctHoles();

    if ( std::fabs(t->z0())>m_z0max ) selected = false;
    if ( std::fabs(t->a0())>m_a0max ) selected = false;
    if ( std::fabs(t->eta())>2.5 || std::fabs(t->pT())<m_pTMin ) selected = false;

    if ( m_type=="loose" ) { 
    
      // Select track silicon hit content
      if( Nsi<7 )              selected = false;
      if( Nsiholes>2 )         selected = false;
      if ( t->pixelHoles()>1 ) selected = false;

      //      if ( m_expectBL && ( ( t->expectBL() || t->hasTruth() ) && t->bLayerHits()<1) )  selected = false;
    }
    else if ( m_type=="loose-primary" ) { 

      // Select track silicon hit content
      if ( ( Nsi<10 ) && ( Nsi<7 || Nsiholes>0 ) ) selected = false;

    }
    else if ( m_type=="tight" || m_type=="vtight" ) { 
      
      // Select track silicon hit content
      if ( std::fabs(t->eta())< 1.65 && Nsi<9  ) selected = false;
      if ( std::fabs(t->eta())>=1.65 && Nsi<11 ) selected = false;
      
      if ( t->pixelHoles()>0 ) selected = false;

      /// require a blayer (ibl in run2) hit only if one is expected
      if ( ( t->expectBL() || t->hasTruth() ) && t->bLayerHits()<1 )  selected = false;

      if ( m_type=="vtight" && t->pixelHits()==0 ) selected = false;
    }
    else if ( m_type=="tight-tau" ) { 

      // Select track silicon hit content
      if ( std::fabs(t->eta())< 1.65 && Nsi<9  ) selected = false;
      if ( std::fabs(t->eta())>=1.65 && Nsi<11 ) selected = false;
      
      if ( t->pixelHoles()>0 ) selected = false;

      if ( t->pixelHits()<4 ) selected = false;

      /// require a blayer (ibl in run2) hit only if one is expected
      if ( ( t->expectBL() || t->hasTruth() ) && t->bLayerHits()<1 )  selected = false;
    }
    else { 
      std::cerr << "Filter_Offline2017::type: " << m_type << "not recognised" << std::endl;
      std::abort();
    }


    return selected;
  } 

private:

  // selection 

  double      m_pTMin;
  double      m_z0max;
  double      m_a0max;
  std::string m_type;

};





inline std::ostream& operator<<( std::ostream& s, const Filter_Offline2017& ) { 
  return s;
}


#endif  // TRIGINDETANALYSISUTILS_FILTER_OFFLINE2017_H 










