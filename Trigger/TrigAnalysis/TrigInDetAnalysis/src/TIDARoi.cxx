//
//   @file    TIDA::Roi.cxx         
//   
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TIDA::Roi.cxx, v0.0   Tue  2 Feb 2010 00:11:09 GMT sutt $


#include "TrigInDetAnalysis/TIDARoi.h"

ClassImp(TIDA::Roi)


TIDA::Roi::Roi() { } 
TIDA::Roi::Roi(const TIDARoiDescriptor& roi) : m_roi(roi) { } 

TIDA::Roi::~Roi() { } 



void TIDA::Roi::addTracks( const std::vector<TIDA::Track*>& trackp) { 
   clear();
   m_tracks.reserve(trackp.size());
   for ( unsigned i=0 ; i<trackp.size() ; i++ )  m_tracks.push_back(*trackp[i]);
}

