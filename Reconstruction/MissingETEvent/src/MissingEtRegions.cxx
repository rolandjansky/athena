/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtRegions.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  S. Resconi
CREATED:  Oct 2004

PURPOSE: Calculate Ex,Ey and Sumet in Eta Regions 
********************************************************************/

#include "MissingETEvent/MissingEtRegions.h"
#include <math.h>

// default contructor
MissingEtRegions::MissingEtRegions()
  : m_reg(MissingEtRegions::Unknown)
{ 
  this->setup();
}


// destructor
MissingEtRegions::~MissingEtRegions()
{ }
/*
// addReg method
void MissingEtRegions::addReg(const CaloCell* aCell, double &weight)
{
  
  double theEx=0.;
  double theEy=0.;
  Calc_Ecellxy(aCell, theEx, theEy);

  double theEta = aCell->eta();
  
  //summing on all eta regions 
  int theReg = EndCap;
  if ( fabs(theEta) <= 1.5 ) theReg = Central ;
  if ( fabs(theEta) >= 3.2 ) theReg = Forward ;
 
  // changed sign to ex and ey sum as in Atrecon 
  
  m_exReg[(size_t)theReg] -= weight * theEx;
  m_eyReg[(size_t)theReg] -= weight * theEy;
  
  // Sumet for each region 
  m_etReg[(size_t)theReg] += weight * aCell->et() ;
 
} */

// addReg method
void MissingEtRegions::addReg(double theEx, double theEy, double theEt, 
			      double theEta, double weight)
{
  // Summing on all eta regions 
  size_t theReg(Forward);
  theEta=fabs(theEta);
  if ( theEta <= 1.5 ) theReg = Central ;
  else {
    if ( theEta <3.2 ) theReg = EndCap ;  
    else {
      if ( fabs(theEta) > 4.5 )
	return;
    }
  }

  m_exReg[theReg] -= weight * theEx;
  m_eyReg[theReg] -= weight * theEy;
  m_etReg[theReg] += weight * theEt ;
 
} 



// retrieve the eta regions
MissingEtRegions::RegionIndex MissingEtRegions::getRegions() 
{
  return m_reg;
}

void MissingEtRegions::setup()
{
  m_exReg.resize(MissingEtRegions::Size,0.);
  m_eyReg.resize(MissingEtRegions::Size,0.);
  m_etReg.resize(MissingEtRegions::Size,0.);
}

// set methods

void MissingEtRegions::setExReg(MissingEtRegions::RegionIndex theReg, 
			      double theEx)
{
  if ( theReg < Size ) m_exReg[(size_t)theReg] = theEx;
}

void MissingEtRegions::setEyReg(MissingEtRegions::RegionIndex theReg, 
			      double theEy)
{
  if ( theReg < Size ) m_eyReg[(size_t)theReg] = theEy;
}

void MissingEtRegions::setEtSumReg(MissingEtRegions::RegionIndex theReg, 
			        double theEtSum)
{
  if ( theReg < Size ) m_etReg[(size_t)theReg] = theEtSum;
}

// get methods

double MissingEtRegions::exReg(MissingEtRegions::RegionIndex theReg) const
{
  return m_exReg[theReg] ;
}

double MissingEtRegions::eyReg(MissingEtRegions::RegionIndex theReg) const
{
  return m_eyReg[theReg] ;
}

double MissingEtRegions::etSumReg(MissingEtRegions::RegionIndex theReg) const
{
  return m_etReg[theReg] ;
}


