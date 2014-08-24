// emacs: this is -*- c++ -*-
//
//   @file    phiHelper.h        
//
//   Copyright (C) 2010 M.Sutton (sutt@cern.ch)    
//
//   $Id: phiHelper.h, v0.0   Sun 20 Jun 2010 20:21:49 BST sutt $


#ifndef PHIHELPER_H
#define PHIHELPER_H

namespace HLT {

inline double wrapPhi( double phi ) { 
    static const double M_2PI = 2*M_PI;  
    while (phi> M_PI) phi -= M_2PI;
    while (phi<-M_PI) phi += M_2PI;
    return phi;
}


inline double phiMean( double phimin, double phimax) { 
    double phi = 0.5*(phimin+phimax);
    if ( phimin>phimax ) phi += M_PI; 
    return wrapPhi(phi);
}


inline double deltaPhi( double phimax, double phimin) { 
  return wrapPhi(phimax-phimin);
}

}


#endif  // __PHIHELPER_H 
