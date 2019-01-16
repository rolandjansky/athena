/* emacs: this is -*- c++ -*- */
/**
 **     @file    Filter_Track.h
 **
 **       generic track filter to filter on kinematics, si and 
 **       trt hit content
 **       NB: Should really make this up from sub filters, ie 
 **       with classes Filter_Param, Filter_Hits...
 **       Filter_Track : Filter_Param, Filter_Hits { 
 **       bool select(..) { 
 **       		       return Filter_Param::select(..) && Filter_Hits::select(..);
 **       }
 **       };                       
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:47:05 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDAUTILS_FILTER_TRACK_H
#define TIDAUTILS_FILTER_TRACK_H

#include <iostream>
#include <cmath>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

/// sadly need to include a root dependency, but no matter - the TIDA::Track 
/// class itself inherets from TObject already, so perhaps the boat has sailed 
/// on that concern long ago
#include "TMath.h"


// generic selection cut selection function 

class Filter_Track : public TrackFilter { 

public:
  
  Filter_Track( double etaMax,  double d0Max,  double z0Max,   double  pTMin,  
		int  minPixelHits, int minSctHits, int minSiHits, int minBlayerHits,  
		int minStrawHits, int minTrHits, double prob=0, 
		int maxPixelHoles=20, int maxSctHoles=20, int maxSiHoles=20, bool expectBL=false ) :
    m_etaMax(etaMax), m_d0Max(d0Max),  m_z0Max(z0Max),  m_pTMin(pTMin), m_pTMax(pTMin-1), // guarantee that the default pTMax is *always* < pTMin  
    m_minPixelHits(minPixelHits),   m_minSctHits(minSctHits),     m_minSiHits(minSiHits),   
    m_minBlayerHits(minBlayerHits), m_minStrawHits(minStrawHits), m_minTrHits(minTrHits),
    m_maxPixelHoles(maxPixelHoles), m_maxSctHoles(maxSctHoles), m_maxSiHoles(maxSiHoles),
    m_prob(prob),
    m_chargeSelection(0),
    m_expectBL(expectBL)
  {   } 

  bool select(const TIDA::Track* t, const TIDARoiDescriptor* =0 ) { 
    // Select track parameters
    bool selected = true;
    if ( std::fabs(t->eta())>m_etaMax || std::fabs(t->a0())>m_d0Max || std::fabs(t->z0())>m_z0Max || std::fabs(t->pT())<m_pTMin ) selected = false;
    if ( m_pTMax>m_pTMin && std::fabs(t->pT())>m_pTMax ) selected = false;
    // Select track silicon hit content
    if( t->pixelHits()<m_minPixelHits || t->sctHits()<m_minSctHits || t->siHits()<m_minSiHits || t->bLayerHits()<m_minBlayerHits ) selected = false;
    if( t->pixelHoles()>m_maxPixelHoles || t->sctHoles()>m_maxSctHoles || (t->pixelHoles()+t->sctHoles())>m_maxSiHoles ) selected = false; 
    // Select track trt hit content
    if( t->strawHits()<m_minStrawHits || t->trHits()<m_minTrHits ) selected = false;
    if( m_prob>0 && TMath::Prob( t->chi2(), t->dof() )<m_prob )    selected = false;
    // track chare selection
    if ( m_chargeSelection!=0 && t->pT()*m_chargeSelection<=0 )  selected = false;
    /// require a blayer (ibl in run2) hit only if one is expected
    if ( m_expectBL && ( ( t->expectBL() || t->hasTruth() ) && t->bLayerHits()<1) )  selected = false;

    return selected;
  } 

  void chargeSelection( int i ) { m_chargeSelection=i; }
  int  chargeSelection() const  { return m_chargeSelection; }

  void   maxpT( double pTMax ) { m_pTMax = pTMax; } 
  double maxpT() const         { return  m_pTMax; } 

private:

  // selection cuts

  double  m_etaMax;
  double  m_d0Max;
  double  m_z0Max;
  double  m_pTMin;
  double  m_pTMax;

  int  m_minPixelHits;
  int  m_minSctHits;
  int  m_minSiHits;
  int  m_minBlayerHits;
  int  m_minStrawHits;
  int  m_minTrHits;

  int  m_maxPixelHoles;
  int  m_maxSctHoles;
  int  m_maxSiHoles;

  double  m_prob;

  int     m_chargeSelection;

  bool    m_expectBL;
};





inline std::ostream& operator<<( std::ostream& s, const Filter_Track& ) { 
  return s;
}


#endif  // TIDAUTILS_FILTER_TRACK_H 










