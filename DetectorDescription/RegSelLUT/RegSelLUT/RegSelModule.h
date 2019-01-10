// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelModule.h  
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Tue Apr  3 11:16:18 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __REGSELMODULE_H
#define __REGSELMODULE_H

#include "Identifier/IdentifierHash.h"
#include "RegSelLUT/ZRObject.h"

#include <iostream>
#include <cmath>
#include <stdint.h> 




class RegSelModule : public ZRObject { 

public:

  RegSelModule();
  /// constructor for a rectangular detector element for 
  /// backwards compatability
  RegSelModule(double zmin,   double zmax, 
	       double rmin,   double rmax, 
	       double phimin, double phimax, 
	       int layer, 
	       int detector, 
	       uint32_t       robid=0, 
	       IdentifierHash hashid=0);

  /// constructor for new trapezoidal detector elements
  RegSelModule(double zmin,   double zmax, 
	       double z2min,  double z2max, 
	       double rmin,   double rmax, 
	       double phimin, double phimax, 
	       int layer, 
	       int detector, 
	       uint32_t       robid=0, 
	       IdentifierHash hashid=0);

#if 0
  bool inRoI(const RegSelRoI& roi) const { 
    // check to see if the RoI and the object overlap in phi   
    // FIXME: need checking for 0-2pi boundary ??? 
    // std::cout << "RegSelModule::inRoI()" << std::endl;
    if ( roi.getphiMax()>phiMin() && roi.getphiMin()<phiMax() ) {
      return ZRObject::inRoI(roi);
    }
    return false;
  }

#else


  // this is a *nasty* function, but it should get all modules
  // in the roi, irrespective of how wide and whether the module
  // and roi cross the pi boundary
  bool inRoI(const RegSelRoI& roi) const { 
    if ( roi.getsplit() ) {    // roi is split across pi
      if ( phiMin()>phiMax() )	return ZRObject::inRoI(roi); // module is also split
      else { // module isn't split                                                 
	if ( ( phiMin()<=M_PI && phiMax()>roi.getphiMin() ) ||
	     ( phiMin()<=roi.getphiMax() && phiMax()>=-M_PI ) ) return ZRObject::inRoI(roi);
      }	
    }
    else {  // roi is not split
      if ( phiMin()<phiMax() ) { // module is not split 
	if ( phiMin()<=roi.getphiMax() && phiMax()>=roi.getphiMin() ) return ZRObject::inRoI(roi);
      }
      else    // module is split NB notice the || rather than the && when not split 
	if ( phiMin()<=roi.getphiMax() || phiMax()>=roi.getphiMin() ) return ZRObject::inRoI(roi);
    }
    return false;
  }

#endif

  double phiMin() const { return m_phiMin; }
  double phiMax() const { return m_phiMax; }

  double phiWidth() const  { 
    return ( m_phiMax>=m_phiMin ? m_phiMax-m_phiMin : m_phiMax-m_phiMin+2*M_PI ); 
  } 

  //  never needed
  //  double etaMin() const { return m_etaMin; }
  //  double etaMax() const { return m_etaMax; }

  int    layer()     const { return m_layer; } 
  int    detector()  const { return m_detector; } 

  uint32_t       robID()  const { return m_robID; }
  IdentifierHash hashID() const { return m_hashID; }

private:

  double m_phiMin, m_phiMax;
  //  double m_etaMin, m_etaMax;  // eta is never used and so not needed
    
  int    m_layer;
  int    m_detector;

  uint32_t       m_robID;
  IdentifierHash m_hashID;
  
};


std::ostream& operator<<(std::ostream& s, const RegSelModule& m); 
bool getModule(std::istream& s,  RegSelModule& m); 


#endif  /* __REGSELMODULE_H */










