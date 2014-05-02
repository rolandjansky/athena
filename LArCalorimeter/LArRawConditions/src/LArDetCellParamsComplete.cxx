/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArDetCellParamsComplete.h" 

LArDetCellParamsComplete::LArDetCellParamsComplete() {}
LArDetCellParamsComplete::~LArDetCellParamsComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArDetCellParamsComplete::set(const HWIdentifier& CellID, int gain, 
				     float omega0, float taur ) {
  
  LArDetCellParamsP t;
  t.m_Omega0       = omega0 ;
  t.m_Taur         = taur ;

  setPdata(CellID,t,gain); 
}


/* retrieve DetCellParams ******************************************************
 */
const float& LArDetCellParamsComplete::Omega0(const HWIdentifier& CellID, int gain) const 
{ 
  const LArDetCellParamsP& t = get(CellID,gain);
  return (t.m_Omega0);
}

const float& LArDetCellParamsComplete::Taur(const HWIdentifier& CellID,int gain) const 
{ 
  const LArDetCellParamsP& t = get(CellID,gain);
  return (t.m_Taur);

}

const float& LArDetCellParamsComplete::Omega0(const Identifier&  CellID, int gain) const
{
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return Omega0(OnId, gain);
}

const float& LArDetCellParamsComplete::Taur(const Identifier&  CellID, int gain) const
{
  const HWIdentifier OnId = larCablingSvc()->createSignalChannelID(CellID); 
  return Taur(OnId, gain);
}


  

