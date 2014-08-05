/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **   @file         RoiDescriptor.cxx 
 **   @author       sutt@cern.ch
 **
 **   Description:  standalone RoIDescriptor implementation  
 **                   
 **   Created:      Tue  5 Aug 2014 00:13:41 CEST
 **   Modified:     
 **************************************************************************/ 

#include "RoiDescriptor/RoiDescriptor.h"

#include <cmath>
#include <sstream>
#include <exception>

/// This should be set by some static class function during the 
/// overall configuration - perhaps by an RoiBuilder class  
const double RoiDescriptor::zedWidthDefault = 225;


#ifndef M_2PI
static const double M_2PI = 2*M_PI;
#endif

#ifndef M_PIF
static const float  M_PIF = float(M_PI);
#endif



RoiDescriptor::RoiDescriptor( bool fullscan )
  : m_phi(0), m_eta(0), m_zed(0), 
    m_phiMinus(0),  m_phiPlus(0),
    m_etaMinus(0),  m_etaPlus(0), 
    m_zedMinus(-zedWidthDefault),  m_zedPlus(zedWidthDefault), 
    m_dzdrMinus(0), m_dzdrPlus(0), 
    m_drdzMinus(0), m_drdzPlus(0), 
    m_fullscan(fullscan),
    m_composite(false),
    m_manageConstituents(false),
    m_version(4)
{
  /// if full scan, give it full detector limits just in 
  /// case anyone doesn't bother to check whether the fullscan 
  /// flag is set
  /// Fixme: these fullscan limits probably need to be set from 
  ///        somewhere constistently. static class variables ???  
  if ( m_fullscan ) construct( 0, -5, 5, 0, -M_PI, M_PI, 0, -zedWidthDefault, zedWidthDefault );
}


RoiDescriptor::RoiDescriptor(double _eta, double _etaMinus, double _etaPlus, 
			     double _phi, double _phiMinus, double _phiPlus, 
			     double _zed, double _zedMinus, double _zedPlus) 
  :  m_phi(phicheck(_phi)), m_eta(etacheck(_eta)), m_zed(zedcheck(_zed)), 
     m_phiMinus(0),  m_phiPlus(0),
     m_etaMinus(0),  m_etaPlus(0), 
     m_zedMinus(0),  m_zedPlus(0), 
     m_dzdrMinus(0), m_dzdrPlus(0), 
     m_drdzMinus(0), m_drdzPlus(0), 
     m_fullscan(false),
     m_composite(false),
     m_manageConstituents(false),
     m_version(4)
{
  construct( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus );
}


RoiDescriptor::RoiDescriptor(double _etaMinus, double _etaPlus, 
			     double _phiMinus, double _phiPlus ) 
  :  m_phi(0), m_eta(0), m_zed(zedcheck(0)), 
     m_phiMinus(0),  m_phiPlus(0),
     m_etaMinus(0),  m_etaPlus(0), 
     m_zedMinus(0),  m_zedPlus(0), 
     m_dzdrMinus(0), m_dzdrPlus(0), 
     m_drdzMinus(0), m_drdzPlus(0), 
     m_fullscan(false),
     m_composite(false),
     m_manageConstituents(false),
     m_version(4)
{
  double _eta = 0.5*(_etaMinus+_etaPlus);
  double _phi = 0.5*(_phiMinus+_phiPlus);
  
  if ( _phiMinus>_phiPlus ) _phi += M_PI;
  if ( _phi<-M_PI ) _phi -= M_2PI;
  if ( _phi>M_PI )  _phi += M_2PI;
 
  construct( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, 0, -zedWidthDefault, zedWidthDefault );
}




RoiDescriptor::~RoiDescriptor(){
  if ( m_composite ) {
    if ( manageConstituents() ) for ( roi_iterator itr=begin() ; itr!=end() ; itr++ ) delete *itr;
  }
}


void RoiDescriptor::construct(double _eta, double _etaMinus, double _etaPlus, 
			      double _phi, double _phiMinus, double _phiPlus, 
			      double _zed, double _zedMinus, double _zedPlus)
{ 
  m_eta = _eta;
  m_phi = _phi;
  m_zed = _zed;

  m_etaPlus  = _etaPlus;
  m_etaMinus = _etaMinus;

  m_zedMinus = _zedMinus;
  m_zedPlus  = _zedPlus;

  // deal with double -> float M_PI conversion 

  m_phiPlus  = _phiPlus; 
  m_phiMinus = _phiMinus;
  
  while ( m_phiPlus> M_PIF ) m_phiPlus -= M_2PI;
  while ( m_phiPlus<-M_PIF ) m_phiPlus += M_2PI;
  /// NB: deal with float to double issue
  ///     This is a hack for the region selector - the RS uses 
  ///     double precision for the phi=pi boundary
  ///     because the RoiDescriptor only uses a float, 
  ///     so pi is represented as 3.14159274 which is > M_PI
  ///     so we have to subtract 1e-7 a bit to prevent 
  ///     it failing any phi>M_PI conditions   
  if ( m_phiPlus> M_PI ) m_phiPlus -= 1e-7; 
  if ( m_phiPlus<-M_PI ) m_phiPlus += 1e-7; 

  m_phiPlus  = _phiPlus; 
  m_phiMinus = _phiMinus;

  while ( m_phiMinus<-M_PIF ) m_phiMinus += M_2PI;
  while ( m_phiMinus> M_PIF ) m_phiMinus -= M_2PI;
  /// NB: see comment in double RoiDescriptor::phiPlus()                                                                                                                                                                         
  if ( m_phiMinus> M_PI ) m_phiMinus -= 1e-7;
  if ( m_phiMinus<-M_PI ) m_phiMinus += 1e-7;


  /// calculate the gradients - very useful these

  m_drdzMinus = std::tan(2*std::atan(std::exp(-_etaMinus)));
  m_drdzPlus  = std::tan(2*std::atan(std::exp(-_etaPlus)));
 
  m_dzdrMinus = 1/m_drdzMinus;
  m_dzdrPlus  = 1/m_drdzPlus;

  const double maxR = 1100; // maximum radius of an ID RoI, hmmm should be a configurable parameter? 

  m_zedOuterMinus = _zedMinus + m_dzdrMinus*maxR;
  m_zedOuterPlus  = _zedPlus  + m_dzdrPlus*maxR;

}
   

/// methods to calculate z position at the RoI boundary 
/// at a given radius
double  RoiDescriptor::zedMin(const double r) const { return  r*m_dzdrMinus+m_zedMinus; } 
double  RoiDescriptor::zedMax(const double r) const { return  r*m_dzdrPlus+m_zedPlus;  } 

double  RoiDescriptor::rhoMin(const double z) const { return  (z-m_zedMinus)*m_drdzMinus; }
double  RoiDescriptor::rhoMax(const double z) const { return  (z-m_zedPlus)*m_drdzPlus ; }


double RoiDescriptor::phicheck(double phi) { 
  while ( phi> M_PIF ) phi-=M_2PI;
  while ( phi<-M_PIF ) phi+=M_2PI;

  if ( !(phi >= -M_PIF && phi <= M_PIF) ) { // use ! of range rather than range to also catch nan etc
    //    printf( "RoiDescriptor: phi range error %20.18f\n", phi );
    //   if ( phi < -M_PIF ) 
    //      throw GaudiException("RoiDescriptor constructed with phi smaller than -PI (allowed range -PI / +PI)", "PhiRange", StatusCode::SUCCESS);
    //   else
    //      throw GaudiException("RoiDescriptor constructed with phi greater than PI (allowed range -PI / +PI)", "PhiRange", StatusCode::SUCCESS);
    throw std::exception(); 
  } 
  return phi;
}


double RoiDescriptor::etacheck(double eta) {
  if ( !(eta > -100  && eta < 100) ) { // check also for nan
    //    std::cout << "RoiDescriptor: eta range error " << eta << std::endl;
    throw std::exception();
    // GaudiException("RoiDescriptor constructed with eta outside range -100 < eta <100", "EtaRange", StatusCode::SUCCESS);
  } 
  return eta;
}


double RoiDescriptor::zedcheck(double zed ) {
  if ( !(zed > -100000  && zed < 100000 ) ) { // check also for nan
    //    std::cout << "RoiDescriptor: zed range error " << zed << std::endl;
    // throw GaudiException("RoiDescriptor constructed with eta outside range -100000 < sed <100000", "ZedRange", StatusCode::SUCCESS);
    throw std::exception();
  } 
  return zed;
}



/// test whether a stub is contained within the roi
bool RoiDescriptor::contains( double z0, double dzdr ) const {
  static const double maxR = 1100; // maximum radius of RoI - outer TRT radius ~1070 mm - should be configurable? 
  double zouter = dzdr*maxR + z0; 
  if ( composite() ) { 
    for ( roi_iterator itr=begin() ; itr!=end() ; itr++ ) if ( (*itr)->contains_internal( z0, zouter ) ) return true;
    return false;
  }
  else return contains_internal( z0, zouter ); 
} 


bool RoiDescriptor::contains_internal( double z0, double zouter ) const {
    if ( z0<=zedPlus() && z0>=zedMinus() && zouter<=m_zedOuterPlus && zouter>=m_zedOuterMinus ) return true;
    return false;
} 
  


/// test whether a stub is contained within the roi
bool RoiDescriptor::containsPhi( double _phi ) const {
  if ( composite() ) { 
      for ( roi_iterator itr=begin() ; itr!=end() ; itr++ ) if ( (*itr)->containsPhi( _phi ) ) return true;
      return false;
  }
  else {
    if ( isFullscan() ) return true; 
    if ( phiPlus()>phiMinus() ) return ( _phi<phiPlus() && _phi>phiMinus() );
    else                        return ( _phi<phiPlus() || _phi>phiMinus() );
  } 
}



bool RoiDescriptor::containsZed( double _z, double _r ) const {
  if ( composite() ) { 
    for ( roi_iterator itr=begin() ; itr!=end() ; itr++ ) if ( (*itr)->containsZed( _z, _r ) ) return true;
    return false;
  }
  else { 
    if ( isFullscan() ) return true;
    double zminus = _r*dzdrMinus() + zedMinus();
    double zplus  = _r*dzdrPlus()  + zedPlus();
    return ( _z>=zminus && _z<=zplus );
  }
}


bool RoiDescriptor::contains( double _z, double _r, double _phi ) const {
  if ( composite() ) { 
    for ( roi_iterator itr=begin() ; itr!=end() ; itr++ ) if ( (*itr)->contains( _z, _r, _phi ) ) return true;
    return false;
  }
  else { 
    return ( containsZed( _z, _r ) && containsPhi(_phi) );
  }
}
  

//////////////////////////////////////////////////////////////////
// helper operators

RoiDescriptor::operator std::string() const {
  std::stringstream ss;
  ss << " z: "   << zed() << " (" <<  zedMinus() << " - " << zedPlus() << ")" 
     << " eta: " << eta() << " (" <<  etaMinus() << " - " << etaPlus() << ")" 
     << " phi: " << phi() << " (" <<  phiMinus() << " - " << phiPlus() << ")" 
     << " RoIid: " << roiId() << " RoIword: " << roiWord();
  if ( composite() ) { 
    ss << "\t : components: " << size() << "\n";
    for ( unsigned i=0 ; i<size() ; i++ ) ss << "\t\t" << i << *at(i) << "\n";
  } 
  return ss.str();
}

std::string str( const RoiDescriptor& d ) {
  return std::string(d);
}

std::ostream& operator<<( std::ostream& m, const RoiDescriptor& d ) {
  return m << std::string(d);
}

