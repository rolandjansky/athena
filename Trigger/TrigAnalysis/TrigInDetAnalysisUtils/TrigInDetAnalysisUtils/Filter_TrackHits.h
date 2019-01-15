/* emacs: this is -*- c++ -*- */
/**
 **     @file    Filter_TrackHits.h
 **
 **     @author  mark sutton
 **     @date    Fri 11 Jan 2019 07:06:39 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisUtils_Filter_TrackHits_H
#define TrigInDetAnalysisUtils_Filter_TrackHits_H


#include <iostream>
#include <string>
#include <cmath>
#include <map>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TrackFilter.h"

#include "TMath.h"


class Filter_TrackHits : public TIDA::TrackFilter { 
  
public:
  
  Filter_TrackHits(int bLayer, int pixel, int sct, int silicon, int straw, int tr, double chi2prob=0.1) :
    m_bLayer(bLayer), m_pixel(pixel), m_sct(sct), m_silicon(silicon), m_straw(straw), m_tr(tr) { } 
  
    bool select(const TIDA::Track* t, const TrigInDetAnalysis::TIDARoiDescriptor* =0) {
      // Select track hit numbers
      if((t->bLayerHits()<m_bLayer) || (t->pixelHits()<m_pixel) || (t->sctHits()<m_sct) || (t->siHits()<m_silicon) ||
	 (t->strawHits()<m_straw) || (t->trHits()<m_tr)) return false;
      return true;
    }
  
prtected:
  
  int m_bLayer;
  int m_pixel;
  int m_sct;
  int m_silicon;
  int m_straw;
  int m_tr;

  double m_probchi2;

};




class Filter_Quality : public Filter_TrackHits { 

public:
  
  Filter_Quality(int bLayer, int pixel, int sct, int silicon, int straw, int tr, double probchi2=0.1) :
    Filter_TrackHits( bLayer, pixel, sct, silicon, straw, tr ) { 
    m_probchi2 = probchi2;  
  }

  bool select(const TIDA::Track* t, const TrigInDetAnalysis::TIDARoiDescriptor* =0) {
    if ( Filter_TrackHits::select( t ) ) return TMath::Prob( tr->chi2(), tf->dof() )>0.1; 
  }

};

#endif  // TrigInDetAnalysisUtils_Filter_TrackHits_H
