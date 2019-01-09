// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    RegSelEtaPhiModule.h        
//
//            a "module" with a region in eta-phi space with max and min
//            coordinates in both eta and phi and a payload.
//            designed so can easily test whether two "modules"
//            overlap, and objects can be sorted by payload.
//  
//
//   $Id: RegSelEtaPhiModule.h, v0.0   Sat 25 Jun 2011 10:30:34 BST sutt $


#ifndef REGSELLUT_REGSELETAPHIMODULE_H
#define REGSELLUT_REGSELETAPHIMODULE_H

#include <iostream>
#include <cmath>


/// base class, with just the eta-phi limits and the 
/// overlap functionality 

class EtaPhiBase {

public:

  EtaPhiBase(double etamin, double etamax, double phimin, double phimax) 
    : m_boundary(false) {

    static const double twopi = 2*M_PI; 
    
    m_eta[0] = etamin;    
    m_eta[1] = etamax;
   
    m_phi[0] = phimin;
    m_phi[1] = phimax;
    
    if ( m_phi[0]<0 ) m_phi[0] += twopi;
    if ( m_phi[1]<0 ) m_phi[1] += twopi;

    if ( m_phi[0]>twopi ) m_phi[0] -= twopi;
    if ( m_phi[1]>twopi ) m_phi[1] -= twopi;

    if ( m_phi[0]>m_phi[1] ) m_boundary = true;
  } 

  /// destructor
  virtual ~EtaPhiBase() = default;

  /// accessors 
  double etamin() const { return m_eta[0]; }
  double etamax() const { return m_eta[1]; }

  double phimin() const { return m_phi[0]; }
  double phimax() const { return m_phi[1]; }


  /// do two eta-phi regions overlap?
  bool overlap(const EtaPhiBase& e) const {

    static const double twopi = 2*M_PI; 

    //    struct timeval timer = simpletimer_start();

    /// eta overlap is trivial
    if ( ! ( e.etamin()<etamax() && e.etamax()>etamin() ) ) return false;    

    /// phi overlap is less trivial, because modules 
    /// can span the 2pi boundary
    bool inphi = false;

    double phimin1 = phimin();
    double phimax1 = phimax();

    double phimin2 = e.phimin();
    double phimax2 = e.phimax();

    // check for phi wrapping and boundary intersection
    if ( phimin1>phimax1 ) phimax1 += twopi;
    if ( phimin2>phimax2 ) phimax2 += twopi;

    if ( ( phimin1<phimax2         && phimax1>phimin2 )         ||
	 ( phimin1<(phimax2+twopi) && phimax1>(phimin2+twopi) ) ||
	 ( (phimin1+twopi)<phimax2 && (phimax1+twopi)>phimin2 ) ) inphi = true; 

    //    double time = simpletimer_stop( timer );

    /// don't need to worry about eta overlap since it is already 
    /// true if we have got to this stage
    return inphi; 
  }

protected:

  //! eta and phi limits for the module
  double  m_eta[2];
  double  m_phi[2];

  bool    m_boundary;

};



inline std::ostream& operator<<( std::ostream& s, const EtaPhiBase& e ) { 
  s << "[ eta=" << e.etamin() << " - " << e.etamax();
  s << "\tphi=" << 180*e.phimin()/M_PI << " - " << 180*e.phimax()/M_PI;
  s<< " ]";
  return s;
}





/// template class with payload added
/// NB: >, <, == and != operators should be defined 
///     for the payload for the sorting when adding to an std::set
///     could get away only with < operator, but there you go 
template<class T>
class TRegSelEtaPhiModule : public EtaPhiBase {

public:

  TRegSelEtaPhiModule(double etamin, double etamax, double phimin, double phimax, const T& t) : 
    EtaPhiBase(etamin, etamax, phimin, phimax), m_t(t) { }
  
  virtual ~TRegSelEtaPhiModule() { } 

  const T& payload() const { return m_t; }
  T&       payload()       { return m_t; }
  
  /// to allow "sorting" and uniqueness tests etc
  /// technically, only need a < operator, but there you go
  template<class E> bool operator<(const TRegSelEtaPhiModule<E>& e)  const {  return payload()<e.payload();  }
  template<class E> bool operator>(const TRegSelEtaPhiModule<E>& e)  const {  return payload()>e.payload(); }
  template<class E> bool operator==(const TRegSelEtaPhiModule<E>& e) const {  return payload()==e.payload(); }
  template<class E> bool operator!=(const TRegSelEtaPhiModule<E>& e) const {  return payload()!=e.payload(); }
 
private:

  /// payload
  T  m_t;

};


/// the payload class needs to have an << streamer defined or
/// we cannot stream it
template<class T>
inline std::ostream& operator<<( std::ostream& s, const TRegSelEtaPhiModule<T>& e ) { 
  s << "[ eta= " << e.etamin() << " - " << e.etamax();
  s << "\tphi= " << 180*e.phimin()/M_PI << " - " << 180*e.phimax()/M_PI;
  s << "\tpay= " << e.payload();
  s << " ]";
  return s;
}



#endif  // REGSELLUT_REGSELETAPHIMODULE_H
