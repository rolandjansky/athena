//
//   @file    TIDARoiDescriptor.cxx         
//   
//            modified from the TrigRoiDescriptor
//                   
// 
//   Copyright (C) 2007 M.Sutton (sutt@cern.ch)    
//
//   $Id: RoiDescriptor.cxx, v0.0   Tue 10 Nov 2009 10:15:40 GMT sutt $


#include <cmath>

#include "TrigInDetAnalysis/TIDARoiDescriptor.h"

ClassImp( TIDARoiDescriptor )


#ifndef M_2PI
#define M_2PI 6.283185307179586476925286766559005768394
#endif

#ifndef M_PIF
static const float  M_PIF = float(M_PI);
#endif

double TIDARoiDescriptor::maxRadius = 1000;
double TIDARoiDescriptor::maxZed    = 2700;


double  TIDARoiDescriptor::exitpoint( double tz0, double teta, double& zexit, double& rexit ) { 

  if      ( teta<0 ) zexit = -maxZed;
  else if ( teta>0 ) zexit =  maxZed;
  else { 
    zexit = tz0;
    rexit = maxRadius; 
    return 1e16; // don't really want to use nan
  }
  
  /// do we need protection against eta=0 ?                                                                                                                                                     

  double tantheta = std::tan( 2*std::atan( std::exp( -teta ) ) );

  rexit = (zexit-tz0) * tantheta;

  /// leaves through the barrel side or front face?                                                                                                                                             
  if ( std::fabs(rexit)>maxRadius ) {
    /// through the barrel edge
    ///  actually need to calculate the z exit coordinate                                                                                                                                       
    ///  for proper containment rather than spurious                                                                                                                                            
    ///  "eta containment"                                                                                                                                                                      
    zexit = maxRadius / tantheta + tz0;
    rexit = maxRadius;

  }

  return tantheta;
}



TIDARoiDescriptor::TIDARoiDescriptor( bool fullscan ) 
  :  m_fullscan(fullscan), 
     m_l1Id(0), m_roiId(0), m_roiWord(0),
     m_zedPlusR(0),
     m_zedMinusR(0),
     m_rPlusZed(0),
     m_rMinusZed(0)
{
  if ( m_fullscan ) { 
    m_params = TIDARoiParameters( 0, -5, 5, 0, -M_PI, M_PI, 0, -225, 225 );

    m_tanthetaPlus  = exitpoint(  225,  5,  m_zedPlusR,  m_rPlusZed );
    m_tanthetaMinus = exitpoint( -225, -5, m_zedMinusR, m_rMinusZed );
  }
}


TIDARoiDescriptor::TIDARoiDescriptor( double eta,  double etaMinus,   double etaPlus,
				      double phi,  double phiMinus,   double phiPlus,
				      double zed,  double zedMinus,   double zedPlus )
  :  m_params( eta, etaMinus, etaPlus, 
	       phi, phiMinus, phiPlus,
	       zed, zedMinus, zedPlus ), 
     m_fullscan(false),
     m_l1Id(0), m_roiId(0), m_roiWord(0)
{ 
  // calculate z limits at radius maxRadius

  m_tanthetaPlus  = exitpoint( this->zedPlus(),  this->etaPlus(),  m_zedPlusR,  m_rPlusZed );
  m_tanthetaMinus = exitpoint( this->zedMinus(), this->etaMinus(), m_zedMinusR, m_rMinusZed );
}


TIDARoiDescriptor::TIDARoiDescriptor( unsigned int l1id, int id, 
				      double eta,  double etaMinus,   double etaPlus,
				      double phi,  double phiMinus,   double phiPlus,
				      double zed,  double zedMinus,   double zedPlus )
  :  m_params( eta, etaMinus, etaPlus, 
	       phi, phiMinus, phiPlus,
	       zed, zedMinus, zedPlus ), 
     m_fullscan(false),
     m_l1Id(l1id), m_roiId(id), m_roiWord(0)
{
  // calculate z limits at radius maxRadius

  m_tanthetaPlus  = exitpoint( this->zedPlus(),  this->etaPlus(),  m_zedPlusR,  m_rPlusZed );
  m_tanthetaMinus = exitpoint( this->zedMinus(), this->etaMinus(), m_zedMinusR, m_rMinusZed );
}


TIDARoiDescriptor::TIDARoiDescriptor(unsigned int roiword, unsigned int l1id, int id, 
				     double eta,  double etaMinus,   double etaPlus,
				     double phi,  double phiMinus,   double phiPlus,
				     double zed,  double zedMinus,   double zedPlus )
  :  m_params( eta, etaMinus, etaPlus, 
	       phi, phiMinus, phiPlus,
	       zed, zedMinus, zedPlus ), 
     m_fullscan(false),
     m_l1Id(l1id), m_roiId(id), m_roiWord(roiword)
{
  // calculate z limits at radius maxRadius
  
  m_tanthetaPlus  = exitpoint( this->zedPlus(),  this->etaPlus(),  m_zedPlusR,  m_rPlusZed );
  m_tanthetaMinus = exitpoint( this->zedMinus(), this->etaMinus(), m_zedMinusR, m_rMinusZed );
}


TIDARoiDescriptor::TIDARoiDescriptor(const TIDARoiDescriptor& a) : 
  TObject(*this), 
  m_params(a.m_params),
  m_fullscan(a.m_fullscan),
  m_l1Id(a.m_l1Id), m_roiId(a.m_roiId), m_roiWord(a.m_roiWord),  
  m_rois(a.m_rois)
{ 
  m_tanthetaPlus  = exitpoint( this->zedPlus(),  this->etaPlus(),  m_zedPlusR,  m_rPlusZed );
  m_tanthetaMinus = exitpoint( this->zedMinus(), this->etaMinus(), m_zedMinusR, m_rMinusZed );
}



TIDARoiDescriptor::~TIDARoiDescriptor() { }


TIDARoiDescriptor::operator std::string() const {
  std::stringstream s;
  s << " z: "   << zed() << " (" << zedMinus() << " - " << zedPlus() << ")"
    << " eta: " << eta() << " (" << etaMinus() << " - " << etaPlus() << ")"
    << " phi: " << phi() << " (" << phiMinus() << " - " << phiPlus() << ")";
  if ( m_fullscan ) s << " (fullscan)";
  s  << " RoIid: " << roiId() << " RoIword: " << roiWord() << " (size " << size() << ")";
  for ( unsigned i=0 ; i<size() ; i++ ) s << "\n\t\t" << i << ": " << (std::string)*at(i);
  return s.str();
}



double TIDARoiDescriptor::phicheck(double phi) {
  if ( !(phi > -M_PIF && phi < M_PIF ) ) { // use ! of range rather than range to also catch nan etc
    if ( phi < -M_PIF )  phi += M_2PI;
    else                 phi -= M_2PI;
    //       std::cerr << "TIDARoiDescriptor constructed with phi smaller than -PI (allowed range -PI / +PI) PhiRange" << std::endl;
    //       std::cerr << "TIDARoiDescriptor constructed with phi greater than PI (allowed range -PI / +PI) PhiRange" << std::endl;
  } 
  return phi;
}


double TIDARoiDescriptor::etacheck(double eta) {
  if ( !(eta > -100  && eta < 100) ) { // check also for nan
    std::cerr << "TIDARoiDescriptor constructed with eta outside range -100 < eta <100  EtaRange" << std::endl;;
  } 
  return eta;
}


double TIDARoiDescriptor::zedcheck(double zed) {
  if ( !(zed > -100000  && zed < 100000 ) ) { // check also for nan
    std::cerr << "TIDARoiDescriptor constructed with eta outside range -100000 < sed <100000  ZedRange" << std::endl;
  } 
  return zed;
}



