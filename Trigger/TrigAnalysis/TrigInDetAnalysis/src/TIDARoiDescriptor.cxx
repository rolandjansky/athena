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


TIDARoiDescriptor::TIDARoiDescriptor()
  : m_phi0(phicheck(0)), m_eta0(etacheck(0)), m_zed0(zedcheck(0)), 
    m_phiHalfWidth(M_PI), m_etaHalfWidth(5), m_zedHalfWidth(225), 
    m_etaPlus(0), m_etaMinus(0), m_l1Id(0), m_roiId(0), m_roiWord(0)
{ }


TIDARoiDescriptor::TIDARoiDescriptor(double eta,  double phi,  double zed, 
			     double etaw, double phiw, double zedw)
  : m_phi0(phicheck(phi)), m_eta0(etacheck(eta)), m_zed0(zedcheck(zed)), 
    m_phiHalfWidth(phiw),  m_etaHalfWidth(etaw),  m_zedHalfWidth(zedw), 
    m_etaPlus(eta), m_etaMinus(eta), m_l1Id(0), m_roiId(0), m_roiWord(0)
{ }


TIDARoiDescriptor::TIDARoiDescriptor(unsigned int l1id, int id, 
			     double eta,  double phi,  double zed, 
			     double etaw, double phiw, double zedw)
  : m_phi0(phicheck(phi)), m_eta0(etacheck(eta)), m_zed0(zedcheck(zed)), 
    m_phiHalfWidth(phiw),  m_etaHalfWidth(etaw),  m_zedHalfWidth(zedw), 
    m_etaPlus(eta), m_etaMinus(eta), m_l1Id(l1id), m_roiId(id), m_roiWord(0)
{ }


TIDARoiDescriptor::TIDARoiDescriptor(unsigned int roiword, unsigned int l1id, int id, 
			     double eta,  double phi,  double zed, 
			     double etaw, double phiw, double zedw)
  : m_phi0(phicheck(phi)), m_eta0(etacheck(eta)), m_zed0(zedcheck(zed)), 
    m_phiHalfWidth(phiw),  m_etaHalfWidth(etaw),  m_zedHalfWidth(zedw), 
    m_etaPlus(eta), m_etaMinus(eta), m_l1Id(l1id), m_roiId(id), m_roiWord(roiword)
{ }


TIDARoiDescriptor::TIDARoiDescriptor(const TIDARoiDescriptor& a) : TObject(*this) {
   *this = a;
}



TIDARoiDescriptor::~TIDARoiDescriptor(){}




double TIDARoiDescriptor::phicheck(double phi) {
  if ( !(phi > -M_PI && phi < M_PI ) ) { // use ! of range rather than range to also catch nan etc
    if ( phi < -M_PI ) 
      std::cerr << "TIDARoiDescriptor constructed with phi smaller than -PI (allowed range -PI / +PI) PhiRange" << std::endl;
    else
      std::cerr << "TIDARoiDescriptor constructed with phi greater than PI (allowed range -PI / +PI) PhiRange" << std::endl;
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



 
