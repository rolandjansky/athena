/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIClusterSubtraction.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"

//**********************************************************************

HIClusterSubtraction::HIClusterSubtraction(std::string name) : asg::AsgTool(name)
{
  declareProperty("ClusterKey", m_cluster_key);
  declareProperty("EventShapeKey",m_event_shape_key);
  declareProperty("Subtractor",m_subtractor_tool);
}

//**********************************************************************


int HIClusterSubtraction::execute() const
{

  //const jet::cellset_t & badcells = badCellMap.cells() ;

  //retrieve UE

  const xAOD::HIEventShapeContainer* shape=0;
  if(evtStore()->retrieve(shape,m_event_shape_key).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve input HIEventShape " << m_event_shape_key );
    return 1;
  }

  const HIEventShapeIndex* es_index=HIEventShapeMap::getIndex(m_event_shape_key);

  xAOD::CaloClusterContainer* ccl=0;
  if(evtStore()->retrieve(ccl,m_cluster_key).isFailure())
  {
    ATH_MSG_ERROR("Could not retrieve input HIEventShape " << m_cluster_key );
    return 1;
  }


  for(xAOD::CaloClusterContainer::iterator itr=ccl->begin(); itr!=ccl->end(); itr++) 
  {
    xAOD::CaloCluster* cl=*itr;
    xAOD::IParticle::FourMom_t p4;
    m_subtractor_tool->Subtract(p4,cl,shape,es_index);

    cl->setAltE(p4.E());
    cl->setAltEta(p4.Eta());
    cl->setAltPhi(p4.Phi());
    cl->setAltM(0);
    
  }
  return 0;
}

