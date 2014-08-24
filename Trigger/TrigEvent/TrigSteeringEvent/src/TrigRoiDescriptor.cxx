/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include <cmath>
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/StatusCode.h"


/**
 * @file TrigRoiDescriptor.cxx
 * @author gianluca.comune@cern.ch
 */
// Feb, 2005 - Modify so that the L1 roiWord can be passed to L2 - R. Soluk  
// Mar. 2007 - Modifications by M. Sutton and T. Bold - enabled Phi check
// Feb. 2008 - Modifications by M. Sutton to enable z extent
// Apr. 2008 - Modifications by M. Sutton to enable Eta and Zed checks


/// This should be set by some static class function during the 
/// overall configuration - perhaps by an RoiBuilder class  
const double TrigRoiDescriptor::zedWidthDefault = 225;



/// non member, non friend interface function 
/// test whether a stub is contained within the roi                                                                                                                                                                  
bool roiContainsZed( const IRoiDescriptor& roi, double _z, double _r ) {
  if ( roi.composite() ) { 
    for ( unsigned int i=0 ; i<roi.size() ; i++ ) if ( roiContainsZed( *roi.at(i), _z, _r ) ) return true;
    return false;
  }
  if ( roi.isFullscan() ) return true;
  double zminus = _r*roi.dzdrMinus()+roi.zedMinus();
  double zplus  = _r*roi.dzdrPlus() +roi.zedPlus();
  return ( _z>=zminus && _z<=zplus );
}



bool roiContains( const IRoiDescriptor& roi, double z, double r, double phi) { 
  if ( roi.composite() ) { 
    for ( unsigned int i=0 ; i<roi.size() ; i++ ) if ( roiContains( *roi.at(i), z, r, phi ) ) return true;
    return false;
  } 
  if ( roi.isFullscan() ) return true;
  return ( roiContainsZed( roi, z, r ) && roi.containsPhi(phi) ); 
}



TrigRoiDescriptor::TrigRoiDescriptor( bool fullscan )
  : m_phi0(0), m_eta0(0), m_zed0(0), 
    m_phiHalfWidth(0), m_etaHalfWidth(0), m_zedHalfWidth(zedWidthDefault), 
    m_dzdrMinus(0), m_dzdrPlus(0), 
    m_drdzMinus(0), m_drdzPlus(0), 
    m_l1Id(0), m_roiId(0), m_roiWord(0),
    m_fullscan(fullscan),
    m_composite(false),
    m_manageConstituents(false),
    m_version(3)
{
  /// if full scan, give it full detector limits just in 
  /// case anyone doesn't bother to check whether the fullscan 
  /// flag is set
  /// Fixme: these fullscan limits probably need to be set from 
  ///        somewhere constistently. static class variables ???  
  if ( m_fullscan ) construct( 0, -5, 5, 0, -M_PI, M_PI, 0, -zedWidthDefault, zedWidthDefault );
}


TrigRoiDescriptor::TrigRoiDescriptor(double _eta, double _etaMinus, double _etaPlus, 
				     double _phi, double _phiMinus, double _phiPlus, 
				     double _zed, double _zedMinus, double _zedPlus) 
  :  m_phi0(phicheck(_phi)), m_eta0(etacheck(_eta)), m_zed0(zedcheck(_zed)), 
     m_phiHalfWidth(0), m_etaHalfWidth(0), m_zedHalfWidth(0), 
     m_dzdrMinus(0), m_dzdrPlus(0), 
     m_drdzMinus(0), m_drdzPlus(0), 
     m_l1Id(0), m_roiId(0), m_roiWord(0),
     m_fullscan(false),
     m_composite(false),
     m_manageConstituents(false),
     m_version(3)
{
  construct( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus );
}


TrigRoiDescriptor::TrigRoiDescriptor(unsigned int l1id, unsigned int id, 
				     double _eta, double _etaMinus, double _etaPlus, 
				     double _phi, double _phiMinus, double _phiPlus, 
				     double _zed, double _zedMinus, double _zedPlus) 
  : m_phi0(phicheck(_phi)), m_eta0(etacheck(_eta)), m_zed0(zedcheck(_zed)), 
    m_phiHalfWidth(0), m_etaHalfWidth(0), m_zedHalfWidth(0),
    m_dzdrMinus(0), m_dzdrPlus(0), 
    m_drdzMinus(0), m_drdzPlus(0),  
    m_l1Id(l1id), m_roiId(id), m_roiWord(0),
    m_fullscan(false),
    m_composite(false),
    m_manageConstituents(false),
    m_version(3)
{
  construct( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus );
}


TrigRoiDescriptor::TrigRoiDescriptor(unsigned int roiWord, unsigned int l1id, unsigned int id, 
				     double _eta, double _etaMinus, double _etaPlus, 
				     double _phi, double _phiMinus, double _phiPlus, 
				     double _zed, double _zedMinus, double _zedPlus) 
  :  m_phi0(phicheck(_phi)), m_eta0(etacheck(_eta)), m_zed0(zedcheck(_zed)), 
     m_phiHalfWidth(0), m_etaHalfWidth(0), m_zedHalfWidth(0),
     m_etaPlus(_eta), m_etaMinus(_eta),
     m_dzdrMinus(0), m_dzdrPlus(0), 
     m_drdzMinus(0), m_drdzPlus(0),  
     m_l1Id(l1id), m_roiId(id), m_roiWord(roiWord),
     m_fullscan(false),
     m_composite(false),
     m_manageConstituents(false),
     m_version(3)
{
  construct( _eta, _etaMinus, _etaPlus, _phi, _phiMinus, _phiPlus, _zed, _zedMinus, _zedPlus );
}



#if 0
TrigRoiDescriptor::TrigRoiDescriptor(const IRoiDescriptor& r)  
  :  m_phi0(r.phi()), m_eta0(r.eta()), m_zed0(r.zed()), 

     m_phiHalfWidth(r.m_phiHalfWidth), m_etaHalfWidth(r.m_etaHalfWidth), m_zedHalfWidth(r.m_zedHalfWidth), 
     m_etaPlus(r.etaPlus()), m_etaMinus(r.etaMinus()),
     m_dzdrMinus(0), m_dzdrPlus(0), 
     m_drdzMinus(0), m_drdzPlus(0), 
     m_l1Id(r.l1Id()), m_roiId(r.roiId()), m_roiWord(r.roiWord()),
     m_fullscan(r.isFullscan()),
     m_composite(r.composite()),
     //     m_manageConstituents(r.manageConstituents()),
     m_version(r.version())
{ 
  //  std::cout << "create TrigRoiDescriptor from old: " << r << " " << *this << std::endl;   
  if ( m_composite ) { 
    // if ( !manageConstituents() ) for ( int i=0 ; i<r.size() ; i++ ) push_back(r.at(i));
    // else { 
       // deep copy
       for ( roi_itr itr=begin() ; itr!=end() ; itr++ ) push_back( new TrigRoiDescriptor( *(dynamic_cast<const TrigRoiDescriptor*>(*itr) ) ) );
       manageConstituents(true);
    // }
  }
}
#endif


TrigRoiDescriptor::~TrigRoiDescriptor(){
  if ( m_composite ) {
    if ( manageConstituents() ) for ( roi_itr itr=begin() ; itr!=end() ; itr++ ) delete *itr;
  }
}


void TrigRoiDescriptor::construct(double _eta, double _etaMinus, double _etaPlus, 
				  double _phi, double _phiMinus, double _phiPlus, 
				  double _zed, double _zedMinus, double _zedPlus)
{ 
  m_eta0 = _eta;
  m_phi0 = _phi;
  m_zed0 = _zed;
  m_etaHalfWidth = 0.5*(_etaPlus-_etaMinus);  
  if ( _phiPlus>_phiMinus )  m_phiHalfWidth = 0.5*(_phiPlus-_phiMinus);
  else                       m_phiHalfWidth = 0.5*(_phiPlus-_phiMinus)+M_PI;
  
  m_zedHalfWidth = 0.5*(_zedPlus-_zedMinus);
  m_etaMinus = _etaMinus;
  m_etaPlus  = _etaPlus;

  /// calculate the gradients - very useful these

  m_drdzMinus = std::tan(2*std::atan(std::exp(-_etaMinus)));
  m_drdzPlus  = std::tan(2*std::atan(std::exp(-_etaPlus)));
 
  m_dzdrMinus = 1/m_drdzMinus;
  m_dzdrPlus  = 1/m_drdzPlus;

  const double maxR = 1100; // maximum radius of an ID RoI, hmmm should be a configurable parameter? 

  m_zedOuterMinus = _zedMinus + m_dzdrMinus*maxR;
  m_zedOuterPlus  = _zedPlus  + m_dzdrPlus*maxR;

}
   


static const double M_2PI = 2*M_PI;
static const float  M_PIF = float(M_PI);

/// for the time being, these are needed to compute the phi 
/// max and min values because they are only stored in a "HalfWidth"
/// variable 
double TrigRoiDescriptor::phiPlus() const {
  double _phiPlus = m_phi0+m_phiHalfWidth;
  while ( _phiPlus> M_PIF ) _phiPlus -= M_2PI;
  while ( _phiPlus<-M_PIF ) _phiPlus += M_2PI;
  /// NB: deal with float to double issue
  ///     This is a hack for the region selector - the RS uses 
  ///     double precision for the phi=pi boundary
  ///     because the RoiDescriptor only uses a float, 
  ///     so pi is represented as 3.14159274 which is > M_PI
  ///     so we have to subtract 1e-7 a bit to prevent 
  ///     it failing any phi>M_PI conditions   
  if ( _phiPlus> M_PI ) _phiPlus -= 1e-7; 
  if ( _phiPlus<-M_PI ) _phiPlus += 1e-7; 
  return _phiPlus;
}


double TrigRoiDescriptor::phiMinus() const {
  double _phiMinus = m_phi0-m_phiHalfWidth;
  while ( _phiMinus<-M_PIF ) _phiMinus += M_2PI;
  while ( _phiMinus> M_PIF ) _phiMinus -= M_2PI;
  /// NB: see comment in double TrigRoiDescriptor::phiPlus() 
  if ( _phiMinus> M_PI ) _phiMinus -= 1e-7; 
  if ( _phiMinus<-M_PI ) _phiMinus += 1e-7; 
  return _phiMinus;
}



/// methods to calculate z position at the RoI boundary 
/// at a given radius
double  TrigRoiDescriptor::zedMin(const double r) const { return  r*m_dzdrMinus+m_zed0-m_zedHalfWidth; } 
double  TrigRoiDescriptor::zedMax(const double r) const { return  r*m_dzdrPlus+m_zed0+m_zedHalfWidth;  } 

double  TrigRoiDescriptor::rhoMin(const double z) const { return  (z-m_zed0+m_zedHalfWidth)*m_drdzMinus; }
double  TrigRoiDescriptor::rhoMax(const double z) const { return  (z-m_zed0-m_zedHalfWidth)*m_drdzPlus ; }


double TrigRoiDescriptor::phicheck(double phi) {
  while ( phi> M_PIF ) phi-=M_2PI;
  while ( phi<-M_PIF ) phi+=M_2PI;

  if ( !(phi >= -M_PIF && phi <= M_PIF) ) { // use ! of range rather than range to also catch nan etc
    //    printf( "TrigRoiDescriptor: phi range error %20.18f\n", phi );
    if ( phi < -M_PIF ) 
      throw GaudiException("TrigRoiDescriptor constructed with phi smaller than -PI (allowed range -PI / +PI)", "PhiRange", StatusCode::SUCCESS);
    else
      throw GaudiException("TrigRoiDescriptor constructed with phi greater than PI (allowed range -PI / +PI)", "PhiRange", StatusCode::SUCCESS);
  } 
  return phi;
}


double TrigRoiDescriptor::etacheck(double eta) {
  if ( !(eta > -100  && eta < 100) ) { // check also for nan
    //    std::cout << "TrigRoiDescriptor: eta range error " << eta << std::endl;
    throw GaudiException("TrigRoiDescriptor constructed with eta outside range -100 < eta <100", "EtaRange", StatusCode::SUCCESS);
  } 
  return eta;
}


double TrigRoiDescriptor::zedcheck(double zed) {
  if ( !(zed > -100000  && zed < 100000 ) ) { // check also for nan
    //    std::cout << "TrigRoiDescriptor: zed range error " << zed << std::endl;
    throw GaudiException("TrigRoiDescriptor constructed with eta outside range -100000 < sed <100000", "ZedRange", StatusCode::SUCCESS);
  } 
  return zed;
}



/// test whether a stub is contained within the roi
bool TrigRoiDescriptor::contains( double z0, double dzdr ) const {
  static const double maxR = 1100; // maximum radius of RoI - outer TRT radius ~1070 mm - should be configurable? 
  double zouter = dzdr*maxR + z0; 
  if ( composite() ) { 
    for ( roi_itr itr=begin() ; itr!=end() ; itr++ ) if ( dynamic_cast<const TrigRoiDescriptor*>(*itr)->contains_internal( z0, zouter ) ) return true;
    return false;
  }
  else return contains_internal( z0, zouter ); 
} 


bool TrigRoiDescriptor::contains_internal( double z0, double zouter ) const {
    if ( z0<=zedPlus() && z0>=zedMinus() && zouter<=m_zedOuterPlus && zouter>=m_zedOuterMinus ) return true;
    return false;
} 
  


/// test whether a stub is contained within the roi
bool TrigRoiDescriptor::containsPhi( double _phi ) const {
  if ( composite() ) { 
      for ( roi_itr itr=begin() ; itr!=end() ; itr++ ) if ( dynamic_cast<const TrigRoiDescriptor*>(*itr)->containsPhi( _phi ) ) return true;
      return false;
  }
  else {
    if ( isFullscan() ) return true; 
    if ( phiPlus()>phiMinus() ) return ( _phi<phiPlus() && _phi>phiMinus() );
    else                        return ( _phi<phiPlus() || _phi>phiMinus() );
  } 
}



bool TrigRoiDescriptor::containsZed( double _z, double _r ) const {
  if ( composite() ) { 
    for ( roi_itr itr=begin() ; itr!=end() ; itr++ ) if ( dynamic_cast<const TrigRoiDescriptor*>(*itr)->containsZed( _z, _r ) ) return true;
    return false;
  }
  else { 
    if ( isFullscan() ) return true;
    double zminus = _r*dzdrMinus() + zedMinus();
    double zplus  = _r*dzdrPlus()  + zedPlus();
    return ( _z>=zminus && _z<=zplus );
  }
}


bool TrigRoiDescriptor::contains( double _z, double _r, double _phi ) const {
  if ( composite() ) { 
    for ( roi_itr itr=begin() ; itr!=end() ; itr++ ) if ( dynamic_cast<const TrigRoiDescriptor*>(*itr)->contains( _z, _r, _phi ) ) return true;
    return false;
  }
  else { 
    return ( containsZed( _z, _r ) && containsPhi(_phi) );
  }
}
  

//////////////////////////////////////////////////////////////////
// helper operators

TrigRoiDescriptor::operator std::string() const {
  std::stringstream ss;
  ss << " z: "   << zed() << " +/- " <<  0.5*(zedPlus()-zedMinus()) 
     << " eta: " << eta() << " +/- " <<  0.5*(etaPlus()-etaMinus()) 
     << " phi: " << phi() << " +/- " <<  0.5*HLT::wrapPhi(phiPlus()-phiMinus()) 
     << " RoIid: " << roiId() << " RoIword: " << roiWord();
  if ( composite() ) { 
    ss << "\t : components: " << size() << "\n";
    for ( unsigned i=0 ; i<size() ; i++ ) ss << "\t\t" << i << *at(i) << "\n";
  } 
  return ss.str();
}

std::string str( const TrigRoiDescriptor& d ) {
  return std::string(d);
}

std::ostream& operator<<( std::ostream& m, const TrigRoiDescriptor& d ) {
  return m << std::string(d);
}

