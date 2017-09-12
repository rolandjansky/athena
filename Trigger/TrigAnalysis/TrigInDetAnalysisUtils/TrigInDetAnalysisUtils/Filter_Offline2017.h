// emacs: this is -*- c++ -*-
//
//   @file    Filter_Offline2017.h        
//
//            Filter class to implement the 2017 CP recomendations
//            on hadronic tracks as found in 
//              https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/TrackingCPPreRecsSummer2017
// 
//   Copyright (C) 2017 M.Sutton (sutt@cern.ch)    
//
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
//    
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//   
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
//   MA  02110-1301, USA.
//


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
  
  Filter_Offline2017(  double pTMin=1000, const std::string& type="tight" ) :  
    m_pTMin(pTMin),
    m_type(type)
  {
    if ( m_type!="loose" && m_type!="loose-primary" && m_type!="tight" ) { 
      std::cerr << "Filter_Offline2017::type: " << m_type << "not recognised" << std::endl;
      std::exit(-1);
    }
  } 

  bool select(const TIDA::Track* t, const TIDARoiDescriptor* =0 ) { 
    // Select track parameters
    bool selected = true;
    
    int Nsi      = t->pixelHits()*0.5 + t->sctHits();
    int Nsiholes = t->pixelHoles()+t->sctHoles();
    
    if ( m_type=="loose" ) { 
      if ( std::fabs(t->eta())>2.5 || std::fabs(t->pT())<m_pTMin ) selected = false;
    
      // Select track silicon hit content
      if( Nsi<7 )              selected = false;
      if( Nsiholes>2 )         selected = false;
      if ( t->pixelHoles()>1 ) selected = false;

      //      if ( m_expectBL && ( ( t->expectBL() || t->hasTruth() ) && t->bLayerHits()<1) )  selected = false;
    }
    else if ( m_type=="loose-primary" ) { 
      if ( std::fabs(t->eta())>2.5 || std::fabs(t->pT())<m_pTMin ) selected = false;

      // Select track silicon hit content
      if ( ( Nsi<10 ) && ( Nsi<7 || Nsiholes>0 ) ) selected = false;


    }
    else if ( m_type=="tight" ) { 
      if ( std::fabs(t->eta())>2.5 || std::fabs(t->pT())<m_pTMin ) selected = false;
      
      // Select track silicon hit content
      if ( std::fabs(t->eta())< 1.65 && Nsi<9  ) selected = false;
      if ( std::fabs(t->eta())>=1.65 && Nsi<11 ) selected = false;
      
      if ( t->pixelHoles()>0 ) selected = false;

      /// require a blayer (ibl in run2) hit only if one is expected
      if ( ( t->expectBL() || t->hasTruth() ) && t->bLayerHits()<1 )  selected = false;
    }

    return selected;
  } 

private:

  // selection 

  double      m_pTMin;
  std::string m_type;

};





inline std::ostream& operator<<( std::ostream& s, const Filter_Offline2017& ) { 
  return s;
}


#endif  // TRIGINDETANALYSISUTILS_FILTER_OFFLINE2017_H 










