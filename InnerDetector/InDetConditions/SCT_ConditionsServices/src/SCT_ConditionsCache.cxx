/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ConditionsCache.cxx
 * @author shaun.roe@cern.ch
**/

#include "SCT_ConditionsCache.h"

void
SCT_ConditionsCache::store(const int idx, const bool status){
  m_map[idx]=status; 
}

std::pair<bool, bool> 
SCT_ConditionsCache::retrieve(const int idx) const{
  bool found(false);
  bool status(false);
  Map_t::const_iterator i(m_map.find(idx));
  if (i==m_map.end()) return std::make_pair(status, found);
  found=true;
  return std::make_pair(i->second, found);
}

int 
SCT_ConditionsCache::size() const {
  return m_map.size(); 
}

void 
SCT_ConditionsCache::reset(){
  m_map.clear(); 
}

