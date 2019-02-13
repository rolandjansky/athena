/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArCableAttenuationComplete.h" 
#include "LArIdentifier/LArOnlineID.h"


LArCableAttenuationComplete::LArCableAttenuationComplete()  {}
LArCableAttenuationComplete::~LArCableAttenuationComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArCableAttenuationComplete::set(const HWIdentifier& CellID, float CableAttenuation ) {
  
  LArCableAttenuationP  t; 
  t.m_CableAttenuation = CableAttenuation ; 
  
  setPdata(CellID, t); 

}

/* retrieve CableAttenuation ******************************************************
 */
const float& LArCableAttenuationComplete::CableAttenuation(const HWIdentifier& CellID) const 
{ 
  const LArCableAttenuationP& t = get( CellID); 
  return (t.m_CableAttenuation);
}



  
