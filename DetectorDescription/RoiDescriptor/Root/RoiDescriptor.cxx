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
#include "IRegionSelector/RoiUtil.h"

#include <cmath>
#include <sstream>
#include <exception>


#ifndef M_2PI
static const double M_2PI = 2*M_PI;
#endif

#ifndef M_PIF
static const float  M_PIF = float(M_PI);
#endif


/// This should be set by some static class function during the 
/// overall configuration - perhaps by an RoiBuilder class  
const double RoiDescriptor::zedWidthDefault = 225;

static int RoiVersion = 4;

RoiDescriptor::RoiDescriptor( bool fullscan )
  : m_phi(0), m_eta(0), m_zed(0), 
    m_phiMinus(0),  m_phiPlus(0),
    m_etaMinus(0),  m_etaPlus(0), 
    m_zedMinus(-zedWidthDefault),  m_zedPlus(zedWidthDefault), 
    m_dzdrMinus(0), m_dzdrPlus(0), 
    m_drdzMinus(0), m_drdzPlus(0), 
    m_zedOuterMinus(0), m_zedOuterPlus(0), 
    m_fullscan(fullscan),
    m_composite(false),
    m_manageConstituents(true),
    m_version(RoiVersion)
{
  /// if full scan, give it full detector limits just in 
  /// case anyone doesn't bother to check whether the fullscan 
  /// flag is set
  /// Fixme: these fullscan limits probably need to be set from 
  ///        somewhere constistently. static class variables ???  
  if ( m_fullscan ) construct( 0, -5, 5, 0, -M_PI, M_PI, 0, -zedWidthDefault, zedWidthDefault );
}


RoiDescriptor::RoiDescriptor(double eta_, double etaMinus_, double etaPlus_, 
			     double phi_, double phiMinus_, double phiPlus_, 
			     double zed_, double zedMinus_, double zedPlus_) 
  :  m_phi(RoiUtil::phicheck(phi_)), m_eta(RoiUtil::etacheck(eta_)), m_zed(RoiUtil::zedcheck(zed_)), 
     m_phiMinus(0),      m_phiPlus(0),
     m_etaMinus(0),      m_etaPlus(0), 
     m_zedMinus(0),      m_zedPlus(0), 
     m_dzdrMinus(0),     m_dzdrPlus(0), 
     m_drdzMinus(0),     m_drdzPlus(0), 
     m_zedOuterMinus(0), m_zedOuterPlus(0), 
     m_fullscan(false),
     m_composite(false),
     m_manageConstituents(true),
     m_version(RoiVersion)
{
  construct( eta_, etaMinus_, etaPlus_, phi_, phiMinus_, phiPlus_, zed_, zedMinus_, zedPlus_ );
}


RoiDescriptor::RoiDescriptor(double etaMinus_, double etaPlus_, 
			     double phiMinus_, double phiPlus_ ) 
  :  m_phi(0), m_eta(0), m_zed(0), 
     m_phiMinus(0),      m_phiPlus(0),
     m_etaMinus(0),      m_etaPlus(0), 
     m_zedMinus(0),      m_zedPlus(0), 
     m_dzdrMinus(0),     m_dzdrPlus(0), 
     m_drdzMinus(0),     m_drdzPlus(0), 
     m_zedOuterMinus(0), m_zedOuterPlus(0), 
     m_fullscan(false),
     m_composite(false),
     m_manageConstituents(true),
     m_version(RoiVersion)
{
  double eta_ = 0.5*(etaMinus_+etaPlus_);
  double phi_ = 0.5*(phiMinus_+phiPlus_);
  
  if ( phiMinus_>phiPlus_ ) phi_ += M_PI;
  if ( phi_<-M_PI ) phi_ -= M_2PI;
  if ( phi_>M_PI )  phi_ += M_2PI;
 
  construct( eta_, etaMinus_, etaPlus_, phi_, phiMinus_, phiPlus_, 0, -zedWidthDefault, zedWidthDefault );
}


RoiDescriptor::RoiDescriptor( const IRoiDescriptor& roi ) : 
  m_phi(roi.phi()), m_eta(roi.eta()), m_zed(roi.zed()), 
  m_phiMinus(0),      m_phiPlus(0),
  m_etaMinus(0),      m_etaPlus(0), 
  m_zedMinus(0),      m_zedPlus(0), 
  m_dzdrMinus(0),     m_dzdrPlus(0), 
  m_drdzMinus(0),     m_drdzPlus(0), 
  m_zedOuterMinus(0), m_zedOuterPlus(0), 
  m_fullscan(roi.isFullscan()),
  m_composite(roi.composite()),
  m_manageConstituents(true), /// always manage constituents
  m_version(roi.version()) { 

  RoiDescriptor::operator=(roi);

}


RoiDescriptor& RoiDescriptor::operator=( const IRoiDescriptor& roi ) {  

  if ( this==&roi ) return *this;
  construct( roi );
  m_fullscan  = roi.isFullscan();
  m_composite = roi.composite();
  m_manageConstituents = true;
  m_version = roi.version();

  if ( roi.size()>0 ) { 
    if ( m_manageConstituents ) { 
      /// manging it's own constituents, so take a deep copy
      for ( unsigned i=0 ; i<roi.size() ; i++ ) push_back( new RoiDescriptor( *roi.at(i) ) );
    }
    else { 
      /// these are already managed elsewhere, just copy the pointers
      for ( unsigned i=0 ; i<roi.size() ; i++ ) push_back( roi.at(i) );      
    }
  }
 
  return *this;
}




RoiDescriptor::~RoiDescriptor(){
  if ( manageConstituents() ) for ( roi_iterator itr=begin() ; itr!=end() ; itr++ ) delete *itr;
}


void RoiDescriptor::construct(double eta_, double etaMinus_, double etaPlus_, 
			      double phi_, double phiMinus_, double phiPlus_, 
			      double zed_, double zedMinus_, double zedPlus_)
{ 
  m_eta = eta_;
  m_phi = phi_;
  m_zed = zed_;

  m_etaPlus  = etaPlus_;
  m_etaMinus = etaMinus_;

  m_zedMinus = zedMinus_;
  m_zedPlus  = zedPlus_;

  // deal with double -> float M_PI conversion 

  m_phiPlus  = phiPlus_; 
  
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


  m_phiMinus = phiMinus_;

  while ( m_phiMinus<-M_PIF ) m_phiMinus += M_2PI;
  while ( m_phiMinus> M_PIF ) m_phiMinus -= M_2PI;
  /// NB: see comment in double RoiDescriptor::phiPlus()                                                                                                                                                                         
  if ( m_phiMinus> M_PI ) m_phiMinus -= 1e-7;
  if ( m_phiMinus<-M_PI ) m_phiMinus += 1e-7;


  /// calculate the gradients - very useful these

  m_drdzMinus = std::tan(2*std::atan(std::exp(-etaMinus_)));
  m_drdzPlus  = std::tan(2*std::atan(std::exp(-etaPlus_)));
 
  m_dzdrMinus = 1/m_drdzMinus;
  m_dzdrPlus  = 1/m_drdzPlus;

  const double maxR = 1100; // maximum radius of an ID RoI, hmmm should be a configurable parameter? 

  m_zedOuterMinus = m_zedMinus + m_dzdrMinus*maxR;
  m_zedOuterPlus  = m_zedPlus  + m_dzdrPlus*maxR;

}


void RoiDescriptor::construct( const IRoiDescriptor& roi ) { 
  construct( roi.eta(),  roi.etaMinus(),  roi.etaPlus(), 
	     roi.phi(),  roi.phiMinus(),  roi.phiPlus(), 
	     roi.zed(),  roi.zedMinus(),  roi.zedPlus() ); 
}

/// methods to calculate z position at the RoI boundary 
/// at a given radius
double  RoiDescriptor::zedMin(const double r) const { return  r*m_dzdrMinus+m_zedMinus; } 
double  RoiDescriptor::zedMax(const double r) const { return  r*m_dzdrPlus+m_zedPlus;  } 

double  RoiDescriptor::rhoMin(const double z) const { return  (z-m_zedMinus)*m_drdzMinus; }
double  RoiDescriptor::rhoMax(const double z) const { return  (z-m_zedPlus)*m_drdzPlus ; }



//////////////////////////////////////////////////////////////////
// helper operators

RoiDescriptor::operator std::string() const {
  std::stringstream ss;
  ss << " z: "   << zed() << " (" <<  zedMinus() << " - " << zedPlus() << ")" 
     << " eta: " << eta() << " (" <<  etaMinus() << " - " << etaPlus() << ")" 
     << " phi: " << phi() << " (" <<  phiMinus() << " - " << phiPlus() << ")"; 
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

