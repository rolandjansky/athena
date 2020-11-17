/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCModuleMap.h"

namespace LVL1TGCCabling8
{

TGCModuleMap::~TGCModuleMap (void) {
  clear();
}
  
int TGCModuleMap::connector (int entry) {
  std::map<int,TGCModuleId*>::iterator iter = m_moduleMap.begin();
  advance(iter,entry);
  return iter->first;
}
  
TGCModuleId* TGCModuleMap::moduleId (int entry) {
  std::map<int,TGCModuleId*>::iterator iter = m_moduleMap.begin();
  advance(iter,entry);
  return iter->second;
}

TGCModuleId* TGCModuleMap::popModuleId (int entry) {
  std::map<int,TGCModuleId*>::iterator iter = m_moduleMap.begin();
  advance(iter,entry);
  TGCModuleId* moduleId = iter->second;
  m_moduleMap.erase(iter);
  return moduleId;
}

void TGCModuleMap::insert (int connector, TGCModuleId* moduleId) {
  if (m_moduleMap.find(connector) == m_moduleMap.end()) {
    m_moduleMap.insert(std::pair<int,TGCModuleId*>(connector,moduleId));
  } else {
    // duplicate with key of connector
    delete moduleId;
  }
  return;
}

int TGCModuleMap::find (int connector) {
  return distance(m_moduleMap.begin(),m_moduleMap.find(connector));
}

int TGCModuleMap::size (void) {
  return m_moduleMap.size();
}

void TGCModuleMap::clear (void) {
  for (auto& p : m_moduleMap) {
    delete p.second;
  }
  m_moduleMap.clear();
  return;
}
  
} // end of namespace
