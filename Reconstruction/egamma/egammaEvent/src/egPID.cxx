/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
double egPID::egammaID(egammaPID::PID key, bool *found) const 
{
  //
  // interface method to retrieve value of pid IsEM or weight
  //

  // retrieve key for cut-based identification for non-isolated objects
  switch(key) {
    
  case egammaPID::SofteIsEM:
  case egammaPID::IsEM:
  case egammaPID::IsEMLoose:
  case egammaPID::IsEMMedium:
  case egammaPID::IsEMTight:
  case egammaPID::IsGoodOQ:
    return (double)egammaIDint(key, found);
  default:
    typedef std::pair<egammaPID::PID,double> elParams;

    std::vector<elParams>::const_iterator p = m_egammaID.begin();
    
    for (;p !=m_egammaID.end(); p++) {
      if ( (*p).first == key ){
	if (found != NULL) {
	  *found = true;
	}
	return (*p).second;
	break;
      }
    }

    if (found != NULL) {
      *found = false;
    }
    return egammaPID::EgPidUndefined;
  }
}

// =================================================================
bool egPID::set_egammaID(egammaPID::PID key, double value) 
{
  //
  // set value of PID for IsEM or weight
  //
  switch(key) {
  case egammaPID::SofteIsEM:
  case egammaPID::IsEM:
  case egammaPID::IsEMLoose:
  case egammaPID::IsEMMedium:
  case egammaPID::IsEMTight:
  case egammaPID::IsGoodOQ:
    return set_egammaIDint( key, (unsigned int) value);
  default:
    typedef std::pair<egammaPID::PID,double> elParams;
    
    std::vector<elParams>::iterator p = m_egammaID.begin();
    
    for (;p !=m_egammaID.end(); p++) {
      if ( (*p).first == key ) break;
    }
    
    if ( p == m_egammaID.end() ) {
      m_egammaID.push_back( elParams(key,value) );
    }
    else {
      (*p).second = value;
    }
    
    return true;
  }
}

// ============================================================
bool egPID::set_egammaIDint(egammaPID::PID key, unsigned int value) 
{
  //
  // method to set value
  //

  typedef std::pair<egammaPID::PID,unsigned int> elParams;

  std::vector<elParams>::iterator p = m_egammaIDint.begin();
 
  for (;p !=m_egammaIDint.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_egammaIDint.end() ) {
    m_egammaIDint.push_back( elParams(key,value) );
  }
  else {
    (*p).second = value;
  }
  return true;
}


// ==================================================
unsigned int egPID::isEM(const unsigned int mask, 
			 egammaPID::PID pid,
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

  return (egammaIDint(egammaPID::SofteIsEM, found) & mask);
}

// ==================================================
unsigned int egPID::IsGoodOQ(const unsigned int mask, bool *found) const 
{
  //
  // apply mask to the IsGoodOQ value
  //

  return (egammaIDint(egammaPID::IsGoodOQ, found) & mask);
}


// ==================================================
unsigned int egPID::egammaIDint(egammaPID::PID key, bool *found) const 
{
  //
  // method to retrieve value
  //
  
  typedef std::pair<egammaPID::PID,unsigned int> elParams;

  std::vector<elParams>::const_iterator p = m_egammaIDint.begin();
 
  for (;p !=m_egammaIDint.end(); p++) {
    if ( (*p).first == key ){
      if (found != NULL) {
	*found = true;
      }
      return (*p).second;
    }
  }
  
  if (found != NULL) {
    *found = false;
  }
  return egammaPID::EgPidUndefined;
}

