/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetEvent/JetMomentMapManager.h"
#include "JetEvent/JetMomentMap.h"

JetMomentMapManager JetMomentMapManager::s_instance;

void JetMomentMapManager::add(JetMomentMap *m){
  
   if (m != &OrphanJetMomentMap::instance ) m_maps.insert(m);
}

void JetMomentMapManager::remove(const JetMomentMap *m){
  if(m_noRemoveMode) return;
  JetMomentMap* map = const_cast<JetMomentMap*>(m);
  std::set<JetMomentMap*>::iterator it= m_maps.find(map);
  if( it != m_maps.end() ) m_maps.erase(it);

}

JetMomentMapManager::~JetMomentMapManager(){
  clear();
}
void JetMomentMapManager::clear(){
  std::set<JetMomentMap*>::iterator it= m_maps.begin();
  std::set<JetMomentMap*>::iterator itE= m_maps.end();  

  m_noRemoveMode = true; // we don't remove anything now (be sure itE doesn't get wrong)
  for(;it !=itE;++it) {
    delete *it;
  }
  m_noRemoveMode = false; 
  m_maps.clear();
}
