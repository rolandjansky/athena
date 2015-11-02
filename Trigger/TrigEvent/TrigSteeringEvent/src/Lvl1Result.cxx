/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSteeringEvent/Lvl1Result.h"

using namespace LVL1CTP;

bool Lvl1Result::anyActiveL1ItemAfterVeto() const
{
  for (unsigned int i = 0; i < m_l1_itemsTAV.size(); ++i) {
    if (m_l1_itemsTAV[i] != 0) return true;
  }

  return false;
}


bool Lvl1Result::isAccepted() const
{
  return ( isConfigured() && anyActiveL1ItemAfterVeto() );
}


bool Lvl1Result::isPassedBeforePrescale(unsigned int item) const
{
  if (item >= m_l1_itemsTBP.size()*32) return 0;
  uint32_t word = m_l1_itemsTBP[item/32];
  return (((word >> (item % 32)) & 0x1) != 0);
}

bool Lvl1Result::isPassedAfterPrescale(unsigned int item) const
{
  if (item >= m_l1_itemsTAP.size()*32) return 0;
  uint32_t word = m_l1_itemsTAP[item/32];
  return (((word >> (item % 32)) & 0x1) != 0);
}

bool Lvl1Result::isPassedAfterVeto(unsigned int item) const
{
  if (item >= m_l1_itemsTAV.size()*32) return 0;
  uint32_t word = m_l1_itemsTAV[item/32];
  return (((word >> (item % 32)) & 0x1) != 0);
}


bool Lvl1Result::isPassedRaw(unsigned int item) const
{
  return isPassedBeforePrescale(item);
}


bool Lvl1Result::isPrescaled(unsigned int item) const
{
  return isPassedBeforePrescale(item) && ! isPassedAfterPrescale(item);
}

bool Lvl1Result::isVeto(unsigned int item) const
{
  return isPassedAfterPrescale(item) && !isPassedAfterVeto(item);
}
