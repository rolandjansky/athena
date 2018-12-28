// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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


#ifndef REGSELLUT_ZROBJECT_H
#define REGSELLUT_ZROBJECT_H

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

  ZRObject() : m_rMin(0),  m_rMax(0),  
	       m_zMin(0),  m_zMax(0), 
	       m_z2Min(0), m_z2Max(0), m_enabled(false)
  { } 
  
  ZRObject(double rmin, double rmax, double zmin, double zmax) :
    m_rMin(rmin<rmax?rmin:rmax),  m_rMax(rmax>rmin?rmax:rmin), 
    m_zMin(zmin<zmax?zmin:zmax),  m_zMax(zmax>zmin?zmax:zmin),
    m_z2Min(zmin<zmax?zmin:zmax), m_z2Max(zmax>zmin?zmax:zmin),
    m_enabled(true)
  {  } 

  ZRObject(double rmin,  double rmax, 
	   double zmin,  double zmax,
	   double z2min, double z2max) :
    m_rMin(rmin<rmax?rmin:rmax),      m_rMax(rmax>rmin?rmax:rmin), 
    m_zMin(zmin<zmax?zmin:zmax),      m_zMax(zmax>zmin?zmax:zmin),
    m_z2Min(z2min<z2max?z2min:z2max), m_z2Max(z2max>z2min?z2max:z2min),
    m_enabled(true)
  {  } 

  virtual ~ZRObject() = default;

  // check to see if the RoI and the object overlap    
  // ok really check if the object is completely outside,
  // otherwise it must overlap
  virtual bool inRoI(const RegSelRoI& roi) const  {   
    if ( !enabled() ) return false; 
    if ( (roi.getzMax(rMin())<zMin() && roi.getzMax(rMax())<z2Min())  || 
	 (roi.getzMin(rMin())>zMax() && roi.getzMin(rMax())>z2Max()) ) {
      return false; 
    }
    return true;
  }
  
  double rMin() const { return m_rMin; } 
  double rMax() const { return m_rMax; } 

  double zMin() const { return m_zMin; } 
  double zMax() const { return m_zMax; } 

  double z2Min() const { return m_z2Min; } 
  double z2Max() const { return m_z2Max; } 

  double rMin(double rmin)  { m_rMin = rmin; return m_rMin; } 
  double rMax(double rmax)  { m_rMax = rmax; return m_rMax; } 

  double zMin(double zmin)  { m_zMin = zmin; return m_zMin; } 
  double zMax(double zmax)  { m_zMax = zmax; return m_zMax; } 

  double z2Min(double zmin)  { m_z2Min = zmin; return m_z2Min; } 
  double z2Max(double zmax)  { m_z2Max = zmax; return m_z2Max; } 

  bool enabled() const  { return m_enabled; } 
  bool enable()  { return m_enabled=true;  } 
  bool disable() { return m_enabled=false; } 

  double _etaMin(double z=0) const { 
    double eta1 = calceta( rMin(), zMin()-z);
    double eta2 = calceta( rMax(), z2Min()-z);
    if ( eta1<eta2 ) return eta1;
    else             return eta2;
  }

  double _etaMax(double z=0) const { 
    double eta1 = calceta( rMin(), zMax()-z);
    double eta2 = calceta( rMax(), z2Max()-z);
    if ( eta1>eta2 ) return eta1;
    else             return eta2;
  }

protected:
  
  double m_rMin,  m_rMax;
  double m_zMin,  m_zMax;
  double m_z2Min, m_z2Max;

  bool  m_enabled;

};


inline std::ostream& operator<<(std::ostream& s, const ZRObject& zr) { 
  s << "[ r=" << zr.rMin() << " - " << zr.rMax() 
    <<  " z=" << zr.zMin() << " - " << zr.zMax();
  if ( zr.zMin()!=zr.z2Min() || zr.zMax()!=zr.z2Max() )  s <<  " z2=" << zr.z2Min() << " - " << zr.z2Max(); 
  s << " ]";
  return s;
}



#endif  /* REGSELLUT_ZROBJECT_H */
