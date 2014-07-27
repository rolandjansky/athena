/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         IDScanSpacePoint.cxx        
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Fri May 26 21:26:22 BST 2006
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 

#ifdef IDSCANDUMP

#include "TrigIDSCAN/IDScanSpacePoint.h"
ClassImp(IDScanSpacePoint)
IDScanSpacePoint::IDScanSpacePoint(double r,  double phi,  double z, 
				   double dr, double dphi, double dz, 
				   long  layer, 
				   double locT, double locL) :
  m_layer(layer), 
  m_r(r),   m_phi(phi),   m_z(z),
  m_dr(dr), m_dphi(dphi), m_dz(dz),
  m_locT(locT), m_locL(locL) 
{  
  m_elementList[0]= 0;
  m_elementList[1]= 0;
  m_width[0]= -1;
  m_width[1]= -1;
  m_stripNumber[0]= -1;
  m_stripNumber[1]= -1;
  m_x = r * cos(phi);
  m_y = r * sin(phi);
  m_barCode=-1;
}
IDScanSpacePoint::IDScanSpacePoint(double r,  double phi,  double z, 
				   double dr, double dphi, double dz, 
				   long  layer, 
				   double locT, double locL, 
				   const unsigned int* elementList,
				   const int* width, const int* strip,
				   int barCode) :
  m_layer(layer), 
  m_r(r),   m_phi(phi),   m_z(z),
  m_dr(dr), m_dphi(dphi), m_dz(dz),
  m_locT(locT), m_locL(locL) 
{  

  m_elementList[0]= elementList[0];
  m_elementList[1]= elementList[1];
  m_width[0]= width[0];
  m_width[1]= width[1];
  m_stripNumber[0]= strip[0];
  m_stripNumber[1]= strip[1];
  m_x = r * cos(phi);
  m_y = r * sin(phi);
  m_barCode = barCode;
} 

ostream& operator<<(ostream& s, const IDScanSpacePoint& sp) {
  return s << "[  r="     << sp.r()     << "+-"   << sp.dr() 
	   << ",\tphi=" << sp.phi()   << "+-"   << sp.dphi() 
	   << ",\tz="   << sp.z()     << "+-"   << sp.dz() 
	   << ",\tlyr=" << sp.layer()  
	   << "   ]"; 
}


#endif /** IDSCANDUMP **/
