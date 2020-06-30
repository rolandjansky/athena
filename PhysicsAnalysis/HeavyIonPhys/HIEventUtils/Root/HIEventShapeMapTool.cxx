/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HIEventShapeMapTool.h"

HIEventShapeMapTool::HIEventShapeMapTool() : asg::AsgTool(){

}

const HIEventShapeIndex* HIEventShapeMapTool::getIndex(std::string key)
{
  auto itr=m_map.find(key);
  if(itr==m_map.end()) return nullptr;
  return &(itr->second);
}

bool HIEventShapeMapTool::hasKey(std::string key)
{
  return (m_map.find(key)!=m_map.end());
}
