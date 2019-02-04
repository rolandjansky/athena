/* emacs: this is -*- c++ -*- */
/**
 **     @file    Filter_TrackParameters.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisUtils_Filter_TrackParameters_H
#define TrigInDetAnalysisUtils_Filter_TrackParameters_H


#include <iostream>
#include <string>
#include <cmath>
#include <map>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackFilter.h"


class Filter_TrackParameters : public TIDA::TrackFilter { 
  
 public:
  
 Filter_TrackParameters(double etaMax,  double d0Max,  double z0Max,   double  pTMin) :
  m_etaMax(etaMax), m_d0Max(d0Max),  m_z0Max(z0Max),  m_pTMin(pTMin) { } 
  
  bool select(const TIDA::Track* t, const TrigInDetAnalysis::TIDARoiDescriptor* r=0) { 
    // Select track parameters
    if (std::fabs(t->eta())>m_etaMax || std::fabs(t->a0())>m_d0Max || std::fabs(t->z0())>m_z0Max || std::fabs(t->pT())<m_pTMin) return false;
    return true;
  } 
  
 private:
  
  double  m_etaMax;
  double  m_d0Max;
  double  m_z0Max;
  double  m_pTMin;

};


#endif  // TrigInDetAnalysisUtils_Filter_TrackParameters_H
