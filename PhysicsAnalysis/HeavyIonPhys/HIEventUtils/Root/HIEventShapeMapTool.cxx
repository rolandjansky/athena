/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODHIEvent/HIEventShape.h"
#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HIEventShapeMapTool.h"

HIEventShapeMapTool::HIEventShapeMapTool(const std::string& n) : asg::AsgTool(n){

}

StatusCode HIEventShapeMapTool::initialize(){

  HIEventShapeIndex index_1, index_2;
  index_1.setBinning(HIEventShapeIndex::COMPACT);
  index_2.setBinning(HIEventShapeIndex::TOWER);
  m_map[ IHIEventShapeMapTool::COMPACT ] = index_1;
  m_map[ IHIEventShapeMapTool::TOWER   ] = index_2;

  return StatusCode::SUCCESS;
}

const HIEventShapeIndex* HIEventShapeMapTool::getIndex(IHIEventShapeMapTool::BinningScheme key) const
{
  std::string myKey;
  if(key == IHIEventShapeMapTool::COMPACT) myKey="COMPACT";
  else if (key == IHIEventShapeMapTool::TOWER ) myKey="TOWER";
  else myKey = "UNKNOWN SHAPE";

  auto itr=m_map.find(key);
  if(itr==m_map.end()) {
    ATH_MSG_ERROR("HIEventShapeMapTool failed to find map w/ format  " << myKey);
    return nullptr;
  }
  return &(itr->second);
}

const HIEventShapeIndex* HIEventShapeMapTool::getIndexFromShape(const xAOD::HIEventShapeContainer* shape) const
{
  ATH_MSG_INFO("In getIndexFromShape, Shape size " << shape->size() << " And TOWER is: " << (HI::TowerBins::numLayers()*HI::TowerBins::numEtaBins()));
  if(shape->size() == IHIEventShapeMapTool::TOWER){
    //TOWER
    auto itr = m_map.find( IHIEventShapeMapTool::TOWER );
    return &(itr->second);
  }
  else if(shape->size() == IHIEventShapeMapTool::COMPACT){
    //COMPACT
    auto itr = m_map.find( IHIEventShapeMapTool::COMPACT );
    return &(itr->second);
  }
  else {
    ATH_MSG_FATAL("Event Shape w/ unknown binning! Not TOWER or COMPACT. Not possible to get Index");
    return nullptr;
  }
}

bool HIEventShapeMapTool::hasKey(IHIEventShapeMapTool::BinningScheme key)
{
  return (m_map.find(key)!=m_map.end());
}
