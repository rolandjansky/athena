/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArCableAttenuationComplete.h" 
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"


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

const float& LArCableAttenuationComplete::CableAttenuation(const Identifier&  CellID) const
{
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return CableAttenuation(OnId);
}


  
