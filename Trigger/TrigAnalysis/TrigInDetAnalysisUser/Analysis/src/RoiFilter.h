/* emacs: this is -*- c++ -*- */
/**
 **     @file    RoiFilter.h
 **
 **     @author  mark sutton
 **     @date    Sun 31 Jul 2016 05:22:41 CEST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  ROIFILTER_H
#define  ROIFILTER_H

#include <iostream>
#include <cmath>

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"


class RoiFilter {

public:

  RoiFilter( double eta=0, bool comp=false, size_t m=1 ) : meta(eta), mcomp(comp), mmult(m) { } 
  RoiFilter(TIDARoiDescriptor) { } 

  double       eta() const { return meta; }
  bool   composite() const { return mcomp; }
  size_t       size() const { return mmult; }

  bool filter( const TIDARoiDescriptor* roi ) { 

    if ( !composite() ) { 
      if ( !roi->composite() && std::fabs(roi->eta())<=eta() ) return true;
      else                                                      return false;
    }

    if ( roi->size()<size() ) return false;
    
    size_t mult = 0;
    for ( size_t i=0 ; i<roi->size() ; i++ ) {
      if ( std::fabs(roi->at(i)->eta())<=eta() ) mult++;
    }
    
    if ( mult<size() ) return false;

    return true;
  } 

  bool filter( const TIDARoiDescriptor& roi ) { return filter( &roi ); }

private:

  double  meta;
  bool    mcomp;
  size_t  mmult;


};

inline std::ostream& operator<<( std::ostream& s, const RoiFilter& r ) { 
  s << "[ RoiFilter: eta: " << r.eta();
  if ( r.composite() ) s << "\tcomposite multiplicity: " << r.size();
  return s << " ]"; 
}


#endif  // ROIFILTER_H 










