/* emacs: this is -*- c++ -*- */
/**
 **     @file    Filter_etaPT.h
 **
 **       generic track filter to filter on eta and PT only
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:47:05 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TRIGINDETANALYSISUTILS_FILTER_ETAPT_H
#define TRIGINDETANALYSISUTILS_FILTER_ETAPT_H

#include <iostream>
#include <cmath>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/Track.h"



// generic selection cut selection function 

class Filter_etaPT : public TrackFilter { 

public:
  
  Filter_etaPT( double etaMax, double  pTMin ) :
    m_etaMax(etaMax), m_pTMin(pTMin)  { 
    //    std::cout << "Filter_etaPT::Filter_etaPT() : etaMax " << m_etaMax << "\tpTMin " << m_pTMin << std::endl;
  } 

  bool select(const TIDA::Track* t, const TIDARoiDescriptor* =0 ) { 
    // Select track parameters
    bool selected = true;
    if ( std::fabs(t->eta())>m_etaMax || std::fabs(t->pT())<m_pTMin ) selected = false;
    return selected;
  } 

private:

  // selection cuts

  double  m_etaMax;
  //double  m_d0Max;
  //double  m_z0Max;
  double  m_pTMin;

};





inline std::ostream& operator<<( std::ostream& s, const Filter_etaPT& /*_s*/ ) { 
  return s;
}


#endif  // TRIGINDETANALYSISUTILS_FILTER_ETAPT_H
