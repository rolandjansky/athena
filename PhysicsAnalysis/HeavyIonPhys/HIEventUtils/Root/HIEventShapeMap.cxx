/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/HIEventShapeMap.h"


HIEventShapeMap HIEventShapeMap::s_obj=HIEventShapeMap();

HIEventShapeMap* HIEventShapeMap::getMap()
{
  return &s_obj;
}

const HIEventShapeIndex* HIEventShapeMap::insert(std::string key, const HIEventShapeIndex& index, bool clobber)
{
  std::map<std::string,HIEventShapeIndex>::iterator mItr=s_obj.m_map.find(key);
  if(mItr!=s_obj.m_map.end())
  {
    if(clobber) mItr->second=index;
  }
  else mItr=s_obj.m_map.insert(std::pair<std::string,HIEventShapeIndex>(key,index)).first;
  return &(mItr->second);
}
const HIEventShapeIndex* HIEventShapeMap::getIndex(std::string key)
{
  auto itr=s_obj.m_map.find(key);
  if(itr==s_obj.m_map.end()) return nullptr;
  return &(itr->second);
}

bool HIEventShapeMap::hasKey(std::string key)
{
  return (s_obj.m_map.find(key)!=s_obj.m_map.end());
}
