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

TIDARoiDescriptor::TIDARoiDescriptor( bool fullscan ) 
  :  m_fullscan(fullscan), 
     m_l1Id(0), m_roiId(0), m_roiWord(0) 
{
  if ( m_fullscan ) m_params = TIDARoiParameters( 0, -5, 5, 0, -M_PI, M_PI, 0, -225, 225 );
}


TIDARoiDescriptor::TIDARoiDescriptor( double eta,  double etaMinus,   double etaPlus,
				      double phi,  double phiMinus,   double phiPlus,
				      double zed,  double zedMinus,   double zedPlus )
  :  m_params( eta, etaMinus, etaPlus, 
	       phi, phiMinus, phiPlus,
	       zed, zedMinus, zedPlus ), 
     m_fullscan(false),
     m_l1Id(0), m_roiId(0), m_roiWord(0)
{ }


TIDARoiDescriptor::TIDARoiDescriptor( unsigned long long l1id, int id, 
				      double eta,  double etaMinus,   double etaPlus,
				      double phi,  double phiMinus,   double phiPlus,
				      double zed,  double zedMinus,   double zedPlus )
  :  m_params( eta, etaMinus, etaPlus, 
	       phi, phiMinus, phiPlus,
	       zed, zedMinus, zedPlus ), 
     m_fullscan(false),
     m_l1Id(l1id), m_roiId(id), m_roiWord(0)
{ }


TIDARoiDescriptor::TIDARoiDescriptor(unsigned int roiword, unsigned long long l1id,
				     int id, 
				     double eta,  double etaMinus,   double etaPlus,
				     double phi,  double phiMinus,   double phiPlus,
				     double zed,  double zedMinus,   double zedPlus )
  :  m_params( eta, etaMinus, etaPlus, 
	       phi, phiMinus, phiPlus,
	       zed, zedMinus, zedPlus ), 
     m_fullscan(false),
     m_l1Id(l1id), m_roiId(id), m_roiWord(roiword)
{ }


TIDARoiDescriptor::TIDARoiDescriptor(const TIDARoiDescriptor& a) : 
  TObject(*this), 
  m_params(a.m_params),
  m_fullscan(a.m_fullscan),
  m_l1Id(a.m_l1Id), m_roiId(a.m_roiId), m_roiWord(a.m_roiWord),  
  m_rois(a.m_rois)
{ }



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



