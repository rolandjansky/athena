/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArWaveDerivedQuantitiesComplete.h" 

LArWaveDerivedQuantitiesComplete::LArWaveDerivedQuantitiesComplete() {}
LArWaveDerivedQuantitiesComplete::~LArWaveDerivedQuantitiesComplete() {}

/* 
 * Fill transient object in ATHENA
 */

void LArWaveDerivedQuantitiesComplete::set(const HWIdentifier& CellID, int gain,
					   float baseline, float maxAmp, 
					   float tmaxAmp,  float width, 
					   float rT0,      float posLobe,
					   float jitter,   unsigned flag)
{
  
  LArWaveDerivedQuantitiesP t ; 
  
  t.m_baseline = baseline;
  t.m_maxAmp   = maxAmp;
  t.m_tmaxAmp  = tmaxAmp;
  t.m_width    = width;
  t.m_rT0      = rT0;
  t.m_posLobe  = posLobe;
  t.m_jitter  = jitter;
  t.m_flag     = flag;

  setPdata(CellID,t,gain) ;
}

/* 
 * retrieve Wave Derived Quantities
 */

const float& LArWaveDerivedQuantitiesComplete::BaseLine(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_baseline);
}

const float& LArWaveDerivedQuantitiesComplete::MaxAmp(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_maxAmp);
}

const float& LArWaveDerivedQuantitiesComplete::TMaxAmp(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_tmaxAmp);
}

const float& LArWaveDerivedQuantitiesComplete::Width(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_width);
}

const float& LArWaveDerivedQuantitiesComplete::rT0(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_rT0);
}

const float& LArWaveDerivedQuantitiesComplete::PosLobe(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_posLobe);
}

const float& LArWaveDerivedQuantitiesComplete::Jitter(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_jitter);
}


const unsigned& LArWaveDerivedQuantitiesComplete::Flag(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_flag);
}
