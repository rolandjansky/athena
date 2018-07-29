// emacs: this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//   @file    phiutils.h        
//
//                   
//  
//
//   $Id: phiutils.h, v0.0   Sun 30 Mar 2014 16:12:42 CEST sutt $


#ifndef  PHIUTILS_H
#define  PHIUTILS_H

#include <cmath>


namespace HLT { 

inline double wrap_phi(double phi) { 
  while ( phi> M_PI ) phi -= 2*M_PI;
  while ( phi<-M_PI ) phi += 2*M_PI;
  return phi;
}

inline double phimean( double phimin, double phimax) { 
  double phi = 0.5*(phimin+phimax);
  if ( phimin>phimax ) phi += M_PI; 
  return wrap_phi(phi);
}

}

#endif  // PHIUTILS_H 










