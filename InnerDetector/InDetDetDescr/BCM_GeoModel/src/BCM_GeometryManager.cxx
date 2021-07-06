/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BCM_GeoModel/BCM_GeometryManager.h"

BCM_GeometryManager::BCM_GeometryManager()
{
  m_on = 0;
}

BCM_ModuleParameters* BCM_GeometryManager::Module(int module_number)
{
  unsigned int mask = (1 << module_number);
  if((mask & m_on) == mask)
    return &(m_moduli[module_number]);
  else
    return nullptr;
}

void BCM_GeometryManager::ModuleOn(int module_number)
{
  m_on = m_on | (1 << module_number);
}

void BCM_GeometryManager::ModuleOff(int module_number)
{
  m_on = m_on & (~(1 << module_number));
}

int BCM_GeometryManager::IsModuleOn(int module_number) const
{
  int mask  = (1 << module_number);
  return mask & m_on;
}
