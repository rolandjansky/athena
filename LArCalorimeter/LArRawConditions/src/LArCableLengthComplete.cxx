/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArCableLengthComplete.h" 

LArCableLengthComplete::LArCableLengthComplete()  {}

LArCableLengthComplete::~LArCableLengthComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArCableLengthComplete::set(const HWIdentifier& CellID, float CableLength ) {
  
  LArCableLengthP  t; 
  t.m_CableLength = CableLength ; 
  
  setPdata(CellID, t); 

}


/* retrieve CableLength ******************************************************
 */
const float& LArCableLengthComplete::CableLength(const HWIdentifier& CellID) const 
{ 
  const LArCableLengthP& t = get( CellID);   
  return (t.m_CableLength);
}

const float& LArCableLengthComplete::CableLength(const Identifier&  CellID) const
{
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return CableLength(OnId);
}


  
