/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArTshaperComplete.h" 

LArTshaperComplete::LArTshaperComplete()  {}

LArTshaperComplete::~LArTshaperComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArTshaperComplete::set(const HWIdentifier& CellID, float Tshaper ) {
  
  LArTshaperP  t; 
  t.m_Tshaper = Tshaper ; 
  
  setPdata(CellID, t); 

}

/* retrieve Tshaper ******************************************************
 */
const float& LArTshaperComplete::Tshaper(const HWIdentifier& CellID) const 
{ 
  const LArTshaperP& t = get( CellID); 
  
  return (t.m_Tshaper);
}

const float& LArTshaperComplete::Tshaper(const Identifier&  CellID) const
{

  const LArTshaperP& t = this->get_offline(CellID); 
  return (t.m_Tshaper);
}
