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
  : mzMin(zMin),     mzMax(zMax), 
    mphiMin(phiMin), mphiMax(phiMax), 
    metaMin(etaMin), metaMax(etaMax),
    msplit(false)
{
  // calculate and store inverses
  minvaMin = std::tan(2*atan(exp(-metaMin)));
  minvaMax = std::tan(2*atan(exp(-metaMax)));
  maMin = 1/minvaMin;
  maMax = 1/minvaMax;

  // AAARGH!!!! Check that the roi is in the correct range 
  double deltaphi = mphiMax-mphiMin;

  if ( mphiMax<mphiMin ) deltaphi+=M_2PI;

  if ( std::fabs(deltaphi-M_2PI)>1e-10 ) { 
    if ( mphiMin> M_PI ) mphiMin -= M_2PI;
    if ( mphiMin<-M_PI ) mphiMin += M_2PI;
    
    if ( mphiMax> M_PI ) mphiMax -= M_2PI;
    if ( mphiMax<-M_PI ) mphiMax += M_2PI;
  }
  else { 
    mphiMin = -M_PI;
    mphiMax =  M_PI;
  }  

  msplit = (mphiMax<mphiMin);

}




std::ostream& operator<<(std::ostream& s, const RegSelRoI& r) { 
  return s << "[ roi: z=" << int(1000*r.getzMin())*0.001   << " - " << int(1000*r.getzMax())*0.001 
	   << "\tphi="    << (r.getphiMin()<0 ? " " : "" ) << int(1000*r.getphiMin())*0.001 << " - " << int(1000*r.getphiMax())*0.001 
	   << "\t( "      << int(10*180*r.getphiMin()/M_PI)*0.1 << " - " << int(10*180*r.getphiMax()/M_PI)*0.1 << " )" 
	   << "\teta="    << int(1000*r.getetaMin())*0.001 << " - " << int(1000*r.getetaMax())*0.001 
	   << " ]"; 
}

















