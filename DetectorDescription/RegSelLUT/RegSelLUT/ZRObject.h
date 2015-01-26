// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         ZRObject.h  
 **
 **   Description:  Object with rmin, rmax, zmin, zmax coordinates  
 **                 with access methods and a test to see whether an 
 **                 RoI lies within it, completely or partially.  
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Wed Apr  4 17:43:46 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 


#ifndef __ZROBJECT_H
#define __ZROBJECT_H

#include "RegSelLUT/RegSelRoI.h"

#include <iostream>


inline double calceta(double r, double z) {
  if ( z==0 ) return 0;
  double theta = std::atan(r/z);
  if ( theta<0 ) theta += M_PI;
  return -std::log( std::tan(theta*0.5) ); 
}


class ZRObject {

public:

  ZRObject() { } 
  
  ZRObject(double rmin, double rmax, double zmin, double zmax) :
    m_rMin(rmin<rmax?rmin:rmax), m_rMax(rmax>rmin?rmax:rmin), 
    m_zMin(zmin<zmax?zmin:zmax), m_zMax(zmax>zmin?zmax:zmin),
    m_enabled(true)
  {  } 
    
  virtual ~ZRObject() { } 

  // check to see if the RoI and the object overlap    
  // ok really check if the object is completely outside,
  // otherwise it must overlap
  virtual bool inRoI(const RegSelRoI& roi) const  {   
    if ( !enabled() ) return false; 
    if ( (roi.getzMax(rMin())<zMin() && roi.getzMax(rMax())<zMin())  || 
	 (roi.getzMin(rMin())>zMax() && roi.getzMin(rMax())>zMax()) ) {
      return false; 
    }
    return true;
  }
  
  double rMin() const { return m_rMin; } 
  double rMax() const { return m_rMax; } 

  double zMin() const { return m_zMin; } 
  double zMax() const { return m_zMax; } 

  double rMin(double rmin)  { m_rMin = rmin; return m_rMin; } 
  double rMax(double rmax)  { m_rMax = rmax; return m_rMax; } 

  double zMin(double zmin)  { m_zMin = zmin; return m_zMin; } 
  double zMax(double zmax)  { m_zMax = zmax; return m_zMax; } 

  bool enabled() const  { return m_enabled; } 
  bool enable()  { return m_enabled=true;  } 
  bool disable() { return m_enabled=false; } 

  double _etaMin(double z=0) const { 
    double eta1 = calceta( rMin(), zMin()-z);
    double eta2 = calceta( rMax(), zMin()-z);
    if ( eta1<eta2 ) return eta1;
    else             return eta2;
  }

  double _etaMax(double z=0) const { 
    double eta1 = calceta( rMin(), zMax()-z);
    double eta2 = calceta( rMax(), zMax()-z);
    if ( eta1>eta2 ) return eta1;
    else             return eta2;
  }

protected:
  
  double m_rMin, m_rMax;
  double m_zMin, m_zMax;

  bool  m_enabled;

};


inline std::ostream& operator<<(std::ostream& s, const ZRObject& zr) { 
  return s << "[ r=" << zr.rMin() << " - " << zr.rMax() 
	   <<  " z=" << zr.zMin() << " - " << zr.zMax() 
	   << " ]";
}



#endif  /* __ZROBJECT_H */










