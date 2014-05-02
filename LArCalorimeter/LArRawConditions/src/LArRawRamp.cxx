/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArRawRamp.h"

LArRawRamp::LArRawRamp(HWIdentifier id, CaloGain::CaloGain gain) : m_ID(id), m_gain(gain) 
{ m_iSat = -1; }

void LArRawRamp::add(const RAMPPOINT_t& point)
{m_ramp.push_back(point);}

void LArRawRamp::setsat(int iSat)
{
  if ((iSat > m_iSat)&&(m_iSat != -1)) return;
  else m_iSat = iSat;
}
