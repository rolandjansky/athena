/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasAverageComplete.h" 

LArMinBiasAverageComplete::LArMinBiasAverageComplete()  {}

LArMinBiasAverageComplete::~LArMinBiasAverageComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArMinBiasAverageComplete::set(const HWIdentifier& CellID, float minBiasAverage ) 
{  
  LArMinBiasAverageP t ; 
  t.m_MinBiasAverage  = minBiasAverage;

  setPdata(CellID,t); 
}

// retrieve Average of E in minimum bias events ****************************
const float& LArMinBiasAverageComplete::minBiasAverage(const HWIdentifier& CellID) const  { 
  const LArMinBiasAverageP& t = get(CellID); 
  return t.m_MinBiasAverage ;
}
