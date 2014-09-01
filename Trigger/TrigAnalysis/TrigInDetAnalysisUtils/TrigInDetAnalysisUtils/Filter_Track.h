// emacs: this is -*- c++ -*-
//
//   @file    Filter_Track.h        
//
//            generic track filter to filter on kinematics, si and 
//            trt hit content
//            NB: Should really make this up from sub filters, ie 
//                with classes Filter_Param, Filter_Hits...
//                   
//                Filter_Track : Filter_Param, Filter_Hits { 
//                   bool select(..) { 
//		       return Filter_Param::select(..) && Filter_Hits::select(..);
//                   }
//                };                       
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: Filter_Track.h, v0.0   Wed 28 Oct 2009 02:47:05 CET sutt $


#ifndef __FILTER_TRACK_H
#define __FILTER_TRACK_H

#include <iostream>
#include <cmath>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"



// generic selection cut selection function 

class Filter_Track : public TrackFilter { 

public:
  
  Filter_Track( double etaMax,  double d0Max,  double z0Max,   double  pTMin,  
		int  minPixelHits, int minSctHits, int minSiHits, int minBlayerHits,  
		int minStrawHits, int minTrHits ) :
    m_etaMax(etaMax), m_d0Max(d0Max),  m_z0Max(z0Max),  m_pTMin(pTMin), 
    m_minPixelHits(minPixelHits),   m_minSctHits(minSctHits),     m_minSiHits(minSiHits),   
    m_minBlayerHits(minBlayerHits), m_minStrawHits(minStrawHits), m_minTrHits(minTrHits) 
  { } 

  bool select(const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* =0 ) { 
    // Select track parameters
    bool selected = true;
    if ( std::fabs(t->eta())>m_etaMax || std::fabs(t->a0())>m_d0Max || std::fabs(t->z0())>m_z0Max || std::fabs(t->pT())<m_pTMin ) selected = false;
    // Select track silicon hit content
    if( t->pixelHits()<m_minPixelHits || t->sctHits()<m_minSctHits || t->siHits()<m_minSiHits || t->bLayerHits()<m_minBlayerHits ) selected = false;
    // Select track trt hit content
    if( t->strawHits()<m_minStrawHits || t->trHits()<m_minTrHits ) selected = false;
    return selected;
  } 

private:

  // selection cuts

  double  m_etaMax;
  double  m_d0Max;
  double  m_z0Max;
  double  m_pTMin;

  int  m_minPixelHits;
  int  m_minSctHits;
  int  m_minSiHits;
  int  m_minBlayerHits;
  int  m_minStrawHits;
  int  m_minTrHits;
  
};





inline std::ostream& operator<<( std::ostream& s, const Filter_Track& ) { 
  return s;
}


#endif  // __FILTER_TRACK_H 










