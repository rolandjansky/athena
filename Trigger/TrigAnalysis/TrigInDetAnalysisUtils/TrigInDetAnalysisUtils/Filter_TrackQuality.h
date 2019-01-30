/* emacs: this is -*- c++ -*- */
/**
 **     @file    Filter_TrackQuality.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisUtils_Filter_TrackQuality_H
#define TrigInDetAnalysisUtils_Filter_TrackQuality_H


#include <iostream>
#include <string>
#include <cmath>
#include <map>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackFilter.h"
#include "TMath.h"



class Filter_TrackQuality : public TrackFilter { 
  
 public:
  
 Filter_TrackQuality(double chi2Prob) :
  m_chi2Prob(chi2Prob) { } 
  
   bool select(const TIDA::Track* t, const TIDARoiDescriptor* /*r=0*/) { 
    // Select track parameters
    int ndof = (t->pixelHits() + float(t->sctHits())/2.)*3 + t->strawHits()*2. - 5;
    double chi2 = t->chi2();
    double prob = TMath::Prob(chi2*float(ndof), ndof);
    std::cout << chi2 << "\t" << ndof << "\t" << prob << std::endl;
    if (prob < m_chi2Prob) return false;
    return true;
  } 
  
 private:
  
  double  m_chi2Prob;

};


#endif  // TrigInDetAnalysisUtils_Filter_TrackQuality_H
