/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 **
 **   File:         RegSelModule.cxx        
 **
 **   Description:    
 **                   
 **                   
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Tue Apr  3 11:16:12 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 



#include "RegSelLUT/RegSelModule.h"

#include <iostream>
#include <cmath>


RegSelModule::RegSelModule() :
  ZRObject( 0, 0, 0, 0 ),
  m_phiMin(0), m_phiMax(0),
  m_id(0), 
  m_layer(0), m_detector(0),
  m_robID(0), m_hashID(0) { }



RegSelModule::RegSelModule(double zmin,   double zmax, 
			   double rmin,   double rmax, 
			   double phimin, double phimax, 
			   int layer, 
			   int detector,
			   uint32_t       robid, 
			   IdentifierHash hashid) : 
  //  ZRObject( std::sqrt(rmin), std::sqrt(rmax), // zmin, zmax ),
  // 	    ( zmin<0 ? -1 : 1 )*std::sqrt(std::fabs(zmin)), 
  //	    ( zmax<0 ? -1 : 1 )*std::sqrt(std::fabs(zmax)) ),
  //  ZRObject( std::log10(rmin)-1.6, std::log10(rmax)-1.6, // zmin, zmax ),
  //	    ( zmin<0 ? -1 : 1 )*std::log10(std::fabs(zmin)), 
  //	    ( zmax<0 ? -1 : 1 )*std::log10(std::fabs(zmax)) ),
  ZRObject( rmin, rmax, zmin, zmax ),
  m_phiMin(phimin), m_phiMax(phimax),
  m_id(0), 
  m_layer(layer), m_detector(detector),
  m_robID(robid), m_hashID(hashid)
{
  while ( m_phiMax> M_PI ) m_phiMax-=2*M_PI; 
  while ( m_phiMax<-M_PI ) m_phiMax+=2*M_PI; 
  while ( m_phiMin> M_PI ) m_phiMin-=2*M_PI; 
  while ( m_phiMin<-M_PI ) m_phiMin+=2*M_PI; 
  //  std::cout << "RegSelModule() " << *this << std::endl;
} 




RegSelModule::RegSelModule(double zmin,   double zmax, 
			   double z2min,  double z2max, 
			   double rmin,   double rmax, 
			   double phimin, double phimax, 
			   int layer, 
			   int detector,
			   uint32_t       robid, 
			   IdentifierHash hashid) : 
  //  ZRObject( std::sqrt(rmin), std::sqrt(rmax), // zmin, zmax ),
  // 	    ( zmin<0 ? -1 : 1 )*std::sqrt(std::fabs(zmin)), 
  //	    ( zmax<0 ? -1 : 1 )*std::sqrt(std::fabs(zmax)) ),
  //  ZRObject( std::log10(rmin)-1.6, std::log10(rmax)-1.6, // zmin, zmax ),
  //	    ( zmin<0 ? -1 : 1 )*std::log10(std::fabs(zmin)), 
  //	    ( zmax<0 ? -1 : 1 )*std::log10(std::fabs(zmax)) ),
  ZRObject( rmin, rmax, zmin, zmax, z2min, z2max ),
  m_phiMin(phimin), m_phiMax(phimax),
  m_id(0), 
  m_layer(layer), m_detector(detector),
  m_robID(robid), m_hashID(hashid)
{
  while ( m_phiMax> M_PI ) m_phiMax-=2*M_PI; 
  while ( m_phiMax<-M_PI ) m_phiMax+=2*M_PI; 
  while ( m_phiMin> M_PI ) m_phiMin-=2*M_PI; 
  while ( m_phiMin<-M_PI ) m_phiMin+=2*M_PI; 
  //  std::cout << "RegSelModule() " << *this << std::endl;
} 



std::ostream& operator<<(std::ostream& s, const RegSelModule& m)
{
  s << "[ lyr= "     << m.layer() 
    << " ,\tdet= "   << m.detector() 
    << " ,\tr= "     << m.rMin()            << " - " << m.rMax()
    << " ,\tphi= "   << m.phiMin()*180/M_PI << " - " << m.phiMax()*180/M_PI
    << " ,\tz= "     << m.zMin()            << " - " << m.zMax();
  if ( m.zMin()!=m.z2Min() || m.zMax()!=m.z2Max() )  s << " ,\tz2= " << m.z2Min() << " - " << m.z2Max();
  s << " ,\trob= 0x"  << std::hex << m.robID() 
    << " ,\thash= 0x" << std::hex << m.hashID() << std::dec << ( m.enabled() ? " ]" : " (disabled)]");
  return s;
}  



bool getModule(std::istream& s, RegSelModule& m)
{
  char _s[128], _s1[128], _s2[128]; 

  int layer;
  int detector;

  double rMin,   rMax;
  double zMin,   zMax;
  double z2Min,  z2Max;
  double phiMin, phiMax;

  uint32_t robid;
  //  IdentifierHash hash;
  unsigned int hashint;

  s >> _s >> _s >> std::dec >> layer 
    >> _s >> _s >> std::dec >> detector 
    >> _s >> _s >> rMin    >> _s >> rMax  
    >> _s >> _s >> phiMin  >> _s >> phiMax  
    >> _s >> _s >> zMin    >> _s >> zMax;
 
  /// copy z limits to max radius values just in case
  z2Min = zMin;
  z2Max = zMax;
  
  s >> _s1 >> _s2;

  /// read in extra z limits if required
  if ( std::string(_s2)=="z2=" ) { 
    s >> z2Min    >> _s >> z2Max;
    s >> _s >> _s;
  }

  s >> std::hex >> robid 
    >> _s >> _s >> std::hex >> hashint >> std::dec >> _s;

  if ( s.fail() ) return false;
  
  //  std::cout << "s.fail() " << s.fail() << std::endl; 

  phiMin *= M_PI/180;   
  phiMax *= M_PI/180;   

  RegSelModule _m( zMin,   zMax, 
		   z2Min,  z2Max, 
		   rMin,   rMax, 
		   phiMin, phiMax, 
		   layer, 
		   detector, 
		   robid, 
		   IdentifierHash(hashint));
  
  m=_m;

  //  std::cout << _m << std::endl;

  return true;
}  


