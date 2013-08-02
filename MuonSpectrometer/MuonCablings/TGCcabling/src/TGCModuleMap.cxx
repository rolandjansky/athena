/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCcabling/TGCModuleMap.h"

namespace LVL1TGCCabling8
{

TGCModuleMap::~TGCModuleMap (void) {
  clear();
}
  
int TGCModuleMap::connector (int entry) {
  std::map<int,TGCModuleId*>::iterator iter = moduleMap.begin();
  advance(iter,entry);
  return iter->first;
}
  
TGCModuleId* TGCModuleMap::moduleId (int entry) {
  std::map<int,TGCModuleId*>::iterator iter = moduleMap.begin();
  advance(iter,entry);
  return iter->second;
}

TGCModuleId* TGCModuleMap::popModuleId (int entry) {
  std::map<int,TGCModuleId*>::iterator iter = moduleMap.begin();
  advance(iter,entry);
  TGCModuleId* moduleId = iter->second;
  moduleMap.erase(iter);
  return moduleId;
}

void TGCModuleMap::insert (int connector, TGCModuleId* moduleId) {
  if (moduleMap.find(connector) == moduleMap.end()) {
    moduleMap.insert(std::pair<int,TGCModuleId*>(connector,moduleId));
  } else {
    // duplicate with key of connector
    delete moduleId;
  }
  return;
}

int TGCModuleMap::find (int connector) {
  return distance(moduleMap.begin(),moduleMap.find(connector));
}

int TGCModuleMap::size (void) {
  return moduleMap.size();
}

void TGCModuleMap::clear (void) {
  std::map<int,TGCModuleId*>::iterator iter;
  std::map<int,TGCModuleId*>::iterator iter_e = moduleMap.end();
  for(iter=moduleMap.begin();iter!=iter_e;iter++){
    delete (iter->second);
  }
  moduleMap.clear();
  return;
}
  
} // end of namespace
