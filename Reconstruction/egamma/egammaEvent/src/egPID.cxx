/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     egPID.cxx
PACKAGE:  offline/Reconstruction/egammaEvent
UPDATED:
          Nov 24, 2009 (FD) use unsigned int
          Jan  5, 2011 (JM) made set_egammaIDint public, added support for multiple isEMs;
	     removed seperate set_isEM and such functions, since not used. (Willing to put
	     them back if people disagree.)
********************************************************************/

// INCLUDE HEADER FILES:

#include "egammaEvent/egPID.h"
#include "GaudiKernel/GaudiException.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructor
// ----------------------------
egPID::egPID()
{}

// =========================================================
// copy constructor
egPID::egPID(const egPID& original)
  : m_egammaIDint (original.m_egammaIDint),  // value as unsigned integer
    m_egammaID (original.m_egammaID)         // value as double
{
}

// =========================================================
// assignment
egPID& egPID::operator=(const egPID& original)
{
  if (this != &original) {
    // value as unsigned integer
    m_egammaIDint = original.m_egammaIDint;
    // value as double
    m_egammaID    = original.m_egammaID;
  }
  return *this;
}

// =========================================================
egPID::~egPID()
{ 
}

// =========================================================
double egPID::egammaID(egammaPIDObs::PID key, bool *found) const 
{
  //
  // interface method to retrieve value of pid IsEM or weight
  //

  // retrieve key for cut-based identification for non-isolated objects
  switch(key) {
    
  case egammaPIDObs::SofteIsEM:
  case egammaPIDObs::IsEM:
  case egammaPIDObs::IsEMLoose:
  case egammaPIDObs::IsEMMedium:
  case egammaPIDObs::IsEMTight:
  case egammaPIDObs::IsGoodOQ:
    return (double)egammaIDint(key, found);
  default:
    typedef std::pair<egammaPIDObs::PID,double> elParams;

    std::vector<elParams>::const_iterator p = m_egammaID.begin();
    
    for (;p !=m_egammaID.end(); ++p) {
      if ( (*p).first == key ){
	if (found != nullptr) {
	  *found = true;
	}
	return (*p).second;
	break;
      }
    }

    if (found != nullptr) {
      *found = false;
    }
    return egammaPIDObs::EgPidUndefined;
  }
}

// =================================================================
bool egPID::set_egammaID(egammaPIDObs::PID key, double value) 
{
  //
  // set value of PID for IsEM or weight
  //
  switch(key) {
  case egammaPIDObs::SofteIsEM:
  case egammaPIDObs::IsEM:
  case egammaPIDObs::IsEMLoose:
  case egammaPIDObs::IsEMMedium:
  case egammaPIDObs::IsEMTight:
  case egammaPIDObs::IsGoodOQ:
    return set_egammaIDint( key, (unsigned int) value);
  default:
    using elParams = std::pair<egammaPIDObs::PID, double>;
    
    std::vector<elParams>::iterator p = m_egammaID.begin();
    
    for (;p !=m_egammaID.end(); ++p) {
      if ( (*p).first == key ) break;
    }
    
    if ( p == m_egammaID.end() ) {
      m_egammaID.emplace_back(key,value );
    }
    else {
      (*p).second = value;
    }
    
    return true;
  }
}

// ============================================================
bool egPID::set_egammaIDint(egammaPIDObs::PID key, unsigned int value) 
{
  //
  // method to set value
  //

  using elParams = std::pair<egammaPIDObs::PID, unsigned int>;

  std::vector<elParams>::iterator p = m_egammaIDint.begin();
 
  for (;p !=m_egammaIDint.end(); ++p) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_egammaIDint.end() ) {
    m_egammaIDint.emplace_back(key,value );
  }
  else {
    (*p).second = value;
  }
  return true;
}


// ==================================================
unsigned int egPID::isEM(const unsigned int mask, 
			 egammaPIDObs::PID pid,
			 bool *found) const 
{
  //
  // apply mask to the IsEM value
  //

  return (egammaIDint(pid, found) & mask);
}

// ==================================================
unsigned int egPID::isEMsofte(const unsigned int mask, bool *found) const 
{
  //
  // apply mask to the SofteIsEM value
  //

  return (egammaIDint(egammaPIDObs::SofteIsEM, found) & mask);
}

// ==================================================
unsigned int egPID::IsGoodOQ(const unsigned int mask, bool *found) const 
{
  //
  // apply mask to the IsGoodOQ value
  //

  return (egammaIDint(egammaPIDObs::IsGoodOQ, found) & mask);
}


// ==================================================
unsigned int egPID::egammaIDint(egammaPIDObs::PID key, bool *found) const 
{
  //
  // method to retrieve value
  //
  
  using elParams = std::pair<egammaPIDObs::PID, unsigned int>;

  std::vector<elParams>::const_iterator p = m_egammaIDint.begin();
 
  for (;p !=m_egammaIDint.end(); ++p) {
    if ( (*p).first == key ){
      if (found != nullptr) {
	*found = true;
      }
      return (*p).second;
    }
  }
  
  if (found != nullptr) {
    *found = false;
  }
  return egammaPIDObs::EgPidUndefined;
}

