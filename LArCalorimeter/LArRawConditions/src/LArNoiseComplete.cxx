/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArNoiseComplete.h" 

LArNoiseComplete::LArNoiseComplete()  
{

}

LArNoiseComplete::~LArNoiseComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArNoiseComplete::set(const HWIdentifier& CellID, int gain, float Noise) {

  LArNoiseP t ; 
  t.m_Noise    =  Noise;
  setPdata(CellID, t, gain); 

}


/* retrieve Noise ******************************************************
 */
const float& LArNoiseComplete::noise(const HWIdentifier& CellID, int gain) const 
{ 

  const LArNoiseP& t  = get(CellID, gain); 
  
  return (t.m_Noise);
}

const float& LArNoiseComplete::noise(const Identifier&  CellID, int gain) const
{

  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return noise(OnId, gain);
}



  

