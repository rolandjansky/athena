// emacs: this is -*- c++ -*-
//
//   @file    Filter_RoiSelector.h        
//
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: Filter_RoiSelector.h, v0.0   Wed 28 Oct 2009 02:44:40 CET sutt $


#ifndef __FILTER_ROISELECTOR_H
#define __FILTER_ROISELECTOR_H

#include <iostream>
#include <cmath>

#include "TrigInDetAnalysis/TrackFilter.h"
#include "TrigInDetAnalysis/Track.h"

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

#ifndef M_2PI
// static const double  M_2PI = 6.28318530717958647692;
static const double  M_2PI = 2*M_PI;
#endif


// roi eta-phi roi based filter - should have z as well for an sensible use

class Filter_RoiSelector : public TrackFilter { 

public:

  Filter_RoiSelector(double emin=0, double emax=0, double pmin=0, double pmax=0 ) : 
    metamin(emin), metamax(emax), 
    mphimin(pmin), mphimax(pmax) 
  { }

  Filter_RoiSelector(const TIDARoiDescriptor& r) : 
    metamin(r.eta()-r.etaHalfWidth()), metamax(r.eta()+r.etaHalfWidth()), 
    mphimin(r.phi()-r.phiHalfWidth()), mphimax(r.phi()-r.phiHalfWidth())
  {
    if ( mphimin<-M_PI ) mphimin+=M_2PI;
    if ( mphimin> M_PI ) mphimin-=M_2PI;
    if ( mphimax<-M_PI ) mphimax+=M_2PI;
    if ( mphimax> M_PI ) mphimax-=M_2PI;
  }

  Filter_RoiSelector(const TIDARoiDescriptor* r) : 
    metamin(r->eta()-r->etaHalfWidth()), metamax(r->eta()+r->etaHalfWidth()), 
    mphimin(r->phi()-r->phiHalfWidth()), mphimax(r->phi()-r->phiHalfWidth())
  {
    if ( mphimin<-M_PI ) mphimin+=M_2PI;
    if ( mphimin> M_PI ) mphimin-=M_2PI;
    if ( mphimax<-M_PI ) mphimax+=M_2PI;
    if ( mphimax> M_PI ) mphimax-=M_2PI;
  }

  bool select(const TrigInDetAnalysis::Track* t, const TIDARoiDescriptor* /*r=0*/ ) { 
    bool inphi = false;
    if ( mphimin<mphimax ) inphi = ( mphimin<t->phi() && t->phi()<mphimax );
    else                   inphi = ( mphimin<t->phi() || t->phi()<mphimax );
    if ( metamin<t->eta() && t->eta()<metamax && inphi ) return true;
    return false;
  }

  double etamin() const { return metamin; }
  double etamax() const { return metamax; }
  double phimin() const { return mphimin; }
  double phimax() const { return mphimax; }

private:

  double metamin, metamax;
  double mphimin, mphimax;

};



inline std::ostream& operator<<( std::ostream& s, const Filter_RoiSelector& sr ) { 
  return s << "[ eta=" << sr.etamin() << " .. " << sr.etamax() << " phi=" << sr.phimin() << " .. " << sr.phimax() << " ]";
}


#endif  // __FILTER_ROISELECTOR_H 










