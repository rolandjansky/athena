// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    Filter_AcceptAll.h        
//
//                   
// 
//
//   $Id: Filter_AcceptAll.h, v0.0   Wed 28 Oct 2009 02:44:40 CET sutt $


#ifndef __FILTER_ACCEPTALL_H
#define __FILTER_ACCEPTALL_H

// #include <iostream>
// #include <cmath>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

//
namespace TIDA { 
class Track;
}

class TIDARoiDescriptor;


class Filter_AcceptAll : public  TrackFilter { 

public:

  Filter_AcceptAll() { } 

  bool select(const TIDA::Track* /*t*/, const TIDARoiDescriptor* /*r=0*/ ) { return true; }

};

#endif  // __FILTER_ACCEPTALL_H 










