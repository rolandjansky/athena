/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrackFilter.h
 **
 **     @brief   base class for a single track selection filter 
 **              allowing parameter setting for complex track 
 **              selection       
 **
 **     @author  mark sutton
 **     @date    Fri 16 Oct 2009 02:11:32 BST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDA_TRACKFILTER_H
#define TIDA_TRACKFILTER_H

#include <iostream>

#include "TrigInDetAnalysis/Track.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

// #include "TrigInDetAnalysis/RoiDescriptorBuilder.h"

class TrackFilter {

public:

  TrackFilter() { } 

  virtual ~TrackFilter() { } 

  //  virtual bool select( const /*TrigInDetAnalysis::*/Track* t ) = 0;
  virtual bool select( const TIDA::Track* t, const TIDARoiDescriptor* r=0 ) = 0; 
  //  virtual bool select( const TIDA::Track* t, const TIDARoiDescriptor* r=0 ) { return select( t ); }

};

inline std::ostream& operator<<( std::ostream& s, const TrackFilter&  ) { 
  return s;
}

#endif  // TIDA_TRACKFILTER_H 










