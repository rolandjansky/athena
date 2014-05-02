/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArRinjComplete.h" 


LArRinjComplete::LArRinjComplete()  {}

LArRinjComplete::~LArRinjComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArRinjComplete::set(const HWIdentifier& CellID, float Rinj ) {
  
  LArRinjP  t; 
  t.m_Rinj = Rinj ; 
  
  setPdata(CellID, t); 

}


/* retrieve Rinj ******************************************************
 */
const float& LArRinjComplete::Rinj(const HWIdentifier& CellID) const 
{ 
  const LArRinjP& t = get( CellID); 
  
  return (t.m_Rinj);
}

const float& LArRinjComplete::Rinj(const Identifier&  CellID) const
{
  const LArRinjP& t = get_offline( CellID); 
  return (t.m_Rinj);
}
 
