// emacs: this is -*- c++ -*-
//
//   @file    TrackFilter.h        
//            base class for a single track selection filter 
//            so you can set parameters for complex track 
//            selection       
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrackFilter.h, v0.0   Fri 16 Oct 2009 02:11:32 BST sutt $


#ifndef __TRACKFILTER_H
#define __TRACKFILTER_H

#include <iostream>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

// #include "TrigInDetAnalysis/RoiDescriptorBuilder.h"

class TrackFilter {

public:

  TrackFilter() { } 

  virtual ~TrackFilter() { } 

  //  virtual bool select( const /*TrigInDetAnalysis::*/Track* t ) = 0;
  virtual bool select( const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* r=0 ) = 0; 
  //  virtual bool select( const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* r=0 ) { return select( t ); }

};

inline std::ostream& operator<<( std::ostream& s, const TrackFilter&  ) { 
  return s;
}

#endif  // __TRACKFILTER_H 










