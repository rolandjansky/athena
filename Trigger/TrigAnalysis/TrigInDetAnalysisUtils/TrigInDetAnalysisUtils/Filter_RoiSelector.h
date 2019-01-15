/* emacs: this is -*- c++ -*- */
/**
 **     @file    Filter_RoiSelector.h
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:44:40 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TRIGINDETANALYSISUTILS_FILTER_ROISELECTOR_H
#define TRIGINDETANALYSISUTILS_FILTER_ROISELECTOR_H

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
    m_etamin(emin), m_etamax(emax), 
    m_phimin(pmin), m_phimax(pmax) 
  { }

  Filter_RoiSelector(const TIDARoiDescriptor& r) : 
    m_etamin(r.eta()-r.etaHalfWidth()), m_etamax(r.eta()+r.etaHalfWidth()), 
    m_phimin(r.phi()-r.phiHalfWidth()), m_phimax(r.phi()-r.phiHalfWidth())
  {
    if ( m_phimin<-M_PI ) m_phimin+=M_2PI;
    if ( m_phimin> M_PI ) m_phimin-=M_2PI;
    if ( m_phimax<-M_PI ) m_phimax+=M_2PI;
    if ( m_phimax> M_PI ) m_phimax-=M_2PI;
  }

  Filter_RoiSelector(const TIDARoiDescriptor* r) : 
    m_etamin(r->eta()-r->etaHalfWidth()), m_etamax(r->eta()+r->etaHalfWidth()), 
    m_phimin(r->phi()-r->phiHalfWidth()), m_phimax(r->phi()-r->phiHalfWidth())
  {
    if ( m_phimin<-M_PI ) m_phimin+=M_2PI;
    if ( m_phimin> M_PI ) m_phimin-=M_2PI;
    if ( m_phimax<-M_PI ) m_phimax+=M_2PI;
    if ( m_phimax> M_PI ) m_phimax-=M_2PI;
  }

  bool select(const TIDA::Track* t, const TIDARoiDescriptor* /*r=0*/ ) { 
    bool inphi = false;
    if ( m_phimin<m_phimax ) inphi = ( m_phimin<t->phi() && t->phi()<m_phimax );
    else                   inphi = ( m_phimin<t->phi() || t->phi()<m_phimax );
    if ( m_etamin<t->eta() && t->eta()<m_etamax && inphi ) return true;
    return false;
  }

  double etamin() const { return m_etamin; }
  double etamax() const { return m_etamax; }
  double phimin() const { return m_phimin; }
  double phimax() const { return m_phimax; }

private:

  double m_etamin, m_etamax;
  double m_phimin, m_phimax;

};



inline std::ostream& operator<<( std::ostream& s, const Filter_RoiSelector& sr ) { 
  return s << "[ eta=" << sr.etamin() << " .. " << sr.etamax() << " phi=" << sr.phimin() << " .. " << sr.phimax() << " ]";
}


#endif  // TRIGINDETANALYSISUTILS_FILTER_ROISELECTOR_H
