/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasComplete.h" 

#include "LArIdentifier/LArOnlineID.h"

#include <iostream> 
using namespace std ;

LArMinBiasComplete::LArMinBiasComplete()  {}

LArMinBiasComplete::~LArMinBiasComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArMinBiasComplete::set(const HWIdentifier& CellID, float minBiasRMS ) 
{  
  LArMinBiasP t ; 
  t.m_MinBiasRMS  = minBiasRMS;

  setPdata(CellID,t); 
}


// retrieve RMS of E in minimum bias events *****************************
const float& LArMinBiasComplete::minBiasRMS(const HWIdentifier& CellID) const 
{ 
  const LArMinBiasP& t = get(CellID);   
  return t.m_MinBiasRMS ;
}

  

