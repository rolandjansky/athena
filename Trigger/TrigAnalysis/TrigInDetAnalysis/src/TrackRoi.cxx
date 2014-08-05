//
//   @file    TrackRoi.cxx         
//   
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrackRoi.cxx, v0.0   Tue  2 Feb 2010 00:11:09 GMT sutt $


#include "TrigInDetAnalysis/TrackRoi.h"

ClassImp(TrackRoi)


TrackRoi::TrackRoi() { } 
TrackRoi::TrackRoi(const TIDARoiDescriptor& roi) : m_roi(roi) { } 

TrackRoi::~TrackRoi() { } 



void TrackRoi::addTracks( const std::vector<TrigInDetAnalysis::Track*>& trackp) { 
   clear();
   m_tracks.reserve(trackp.size());
   for ( unsigned i=0 ; i<trackp.size() ; i++ )  m_tracks.push_back(*trackp[i]);
}

