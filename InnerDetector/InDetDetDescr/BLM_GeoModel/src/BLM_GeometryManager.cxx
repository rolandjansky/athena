/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BLM_GeoModel/BLM_GeometryManager.h"

BLM_GeometryManager::BLM_GeometryManager()
{
  m_on = 0;
}

BLM_ModuleParameters* BLM_GeometryManager::Module(unsigned int module_number)
{
  unsigned int mask = (1 << module_number);
  if((mask & m_on) == mask)
    return &(m_moduli[module_number]);
  else
    return NULL;
}

void BLM_GeometryManager::ModuleOn(unsigned int module_number)
{
  m_on = m_on | (1 << module_number);
}

void BLM_GeometryManager::ModuleOff(unsigned int module_number)
{
  m_on = m_on & (!(1 << module_number));
}

unsigned int BLM_GeometryManager::IsModuleOn(unsigned int module_number) const
{
  unsigned int mask  = (1 << module_number);
  return mask & m_on;
}
