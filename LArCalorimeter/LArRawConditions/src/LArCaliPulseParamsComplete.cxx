/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArCaliPulseParamsComplete.h" 

LArCaliPulseParamsComplete::LArCaliPulseParamsComplete()  
{}

LArCaliPulseParamsComplete::~LArCaliPulseParamsComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArCaliPulseParamsComplete::set(const HWIdentifier& CellID, int gain, 
				     float tcal, float fstep ) {
  
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;

  setPdata(CellID,t,gain) ;
}
void LArCaliPulseParamsComplete::set(const HWIdentifier& CellID, int gain, 
				     float tcal, float fstep, float offset, float dtcal ) {
  
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;
  t.m_Offset       = offset ;
  t.m_dTimeCal     = dtcal ;

  setPdata(CellID,t,gain) ;
}
void LArCaliPulseParamsComplete::set(const HWIdentifier& CellID, int gain, 
				     float tcal, float fstep, float offset, float dtcal, short ncb ) {
  
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;
  t.m_Offset       = offset ;
  t.m_dTimeCal     = dtcal ;
  t.m_nCB          = ncb ;

  setPdata(CellID,t,gain) ;
}


/* retrieve CaliPulseParams ******************************************************
 */
const float& LArCaliPulseParamsComplete::Tcal(const HWIdentifier& CellID, int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_Tcal);
}

const float& LArCaliPulseParamsComplete::Fstep(const HWIdentifier& CellID,int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_Fstep);
}

const float& LArCaliPulseParamsComplete::Offset(const HWIdentifier& CellID,int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_Offset);
}

const float& LArCaliPulseParamsComplete::dTimeCal(const HWIdentifier& CellID,int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_dTimeCal);
}

const short& LArCaliPulseParamsComplete::nCB(const HWIdentifier& CellID,int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_nCB);
}
