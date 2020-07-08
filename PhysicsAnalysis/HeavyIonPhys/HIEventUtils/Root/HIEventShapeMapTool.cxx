/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HIEventShapeMapTool.h"

HIEventShapeMapTool::HIEventShapeMapTool(const std::string& n) : asg::AsgTool(n){

}

StatusCode HIEventShapeMapTool::initialize(){
  //The idea is that the tool is being configured w/ all the necessary info and intialiazed accordingly
  //This is to prevent access and changes afterwards, that have to be mutexed
  HIEventShapeIndex index_1;
  index_1.setBinning(HIEventShapeIndex::COMPACT);
  //What was done before at level of HIEventShapeFillerTool
  ATH_CHECK(insert(m_containerName,index_1));
  //Now the HIEventShapeJetIteration - here we depend from the initialization flags
  std::vector < HIEventShapeIndex > index_i;
  if( m_useCaloCell == true || (m_useCaloCell == false && m_useClusters == false) ){
    for(int i = 0; i < (int)m_JetIterNames.size(); i++)
    {
      index_i.push_back(HIEventShapeIndex());
      index_i.back().setBinning(HIEventShapeIndex::COMPACT);
      ATH_CHECK(insert(m_JetIterNames[i],index_i.back()));
    }
  }
  else if ( m_useCaloCell == false && m_useClusters == true ){
    for(int i = 0; i < (int)m_JetIterNames.size(); i++)
    {
      index_i.push_back(HIEventShapeIndex());
      index_i.back().setBinning(HIEventShapeIndex::TOWER);
      ATH_CHECK(insert(m_JetIterNames[i],index_i.back()));
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode HIEventShapeMapTool::insert(std::string key, const HIEventShapeIndex& index, bool clobber)
{
  std::map<std::string,HIEventShapeIndex>::iterator mItr=getMap().find(key);
  if(mItr!=getMap().end())
  {
    if(clobber) mItr->second=index;
  }
  else mItr=getMap().insert(std::pair<std::string,HIEventShapeIndex>(key,index)).first;
  //return &(mItr->second);
  return StatusCode::SUCCESS;
}

const HIEventShapeIndex* HIEventShapeMapTool::getIndex(std::string key) const
{
  auto itr=getMap().find(key);
  if(itr==getMap().end()) return nullptr;
  return &(itr->second);
}

bool HIEventShapeMapTool::hasKey(std::string key)
{
  return (getMap().find(key)!=getMap().end());
}
