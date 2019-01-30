/* emacs: this is -*- c++ -*- */
/**
 **     @file    Filter_AcceptAll.h
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:44:40 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TIDA_FILTER_ACCEPTALL_H
#define TIDA_FILTER_ACCEPTALL_H

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

#endif  // TIDA_FILTER_ACCEPTALL_H 










