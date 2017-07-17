/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         RegSelRoI.cxx        
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Thu Feb 15 00:19:57 GMT 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 

#include "RegSelLUT/RegSelRoI.h"

#include <cmath>

#include <iostream>

static const double M_2PI = 2*M_PI;

RegSelRoI::RegSelRoI(double zMin,   double zMax, 
		     double phiMin, double phiMax, 
		     double etaMin, double etaMax) 
  : m_zMin(zMin),     m_zMax(zMax), 
    m_phiMin(phiMin), m_phiMax(phiMax), 
    m_etaMin(etaMin), m_etaMax(etaMax),
    m_split(false)
{
  // calculate and store inverses
  m_invaMin = std::tan(2*atan(exp(-m_etaMin)));
  m_invaMax = std::tan(2*atan(exp(-m_etaMax)));
  m_aMin = 1/m_invaMin;
  m_aMax = 1/m_invaMax;

  // AAARGH!!!! Check that the roi is in the correct range 
  double deltaphi = m_phiMax-m_phiMin;

  if ( m_phiMax<m_phiMin ) deltaphi+=M_2PI;

  if ( std::fabs(deltaphi-M_2PI)>1e-10 ) { 
    if ( m_phiMin> M_PI ) m_phiMin -= M_2PI;
    if ( m_phiMin<-M_PI ) m_phiMin += M_2PI;
    
    if ( m_phiMax> M_PI ) m_phiMax -= M_2PI;
    if ( m_phiMax<-M_PI ) m_phiMax += M_2PI;
  }
  else { 
    m_phiMin = -M_PI;
    m_phiMax =  M_PI;
  }  

  m_split = (m_phiMax<m_phiMin);

}




std::ostream& operator<<(std::ostream& s, const RegSelRoI& r) { 
  return s << "[ roi: z=" << int(1000*r.getzMin())*0.001   << " - " << int(1000*r.getzMax())*0.001 
	   << "\tphi="    << (r.getphiMin()<0 ? " " : "" ) << int(1000*r.getphiMin())*0.001 << " - " << int(1000*r.getphiMax())*0.001 
	   << "\t( "      << int(10*180*r.getphiMin()/M_PI)*0.1 << " - " << int(10*180*r.getphiMax()/M_PI)*0.1 << " )" 
	   << "\teta="    << int(1000*r.getetaMin())*0.001 << " - " << int(1000*r.getetaMax())*0.001 
	   << " ]"; 
}

















