/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIClusterSubtraction.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIJetRec/HIJetRecDefs.h"

//**********************************************************************

HIClusterSubtraction::HIClusterSubtraction(std::string name) : asg::AsgTool(name), 
							       m_clusterCorrectionTools(this)
{
  declareProperty("ClusterKey", m_cluster_key);
  declareProperty("EventShapeKey",m_event_shape_key);
  declareProperty("Subtractor",m_subtractor_tool);
  declareProperty("Modulator",m_modulator_tool);
  declareProperty("SetMoments",m_set_moments=true);
  declareProperty("ClusterCorrectionTools",m_clusterCorrectionTools);
  
}

//**********************************************************************

StatusCode HIClusterSubtraction::initialize()
{
  for (auto tool :  m_clusterCorrectionTools) 
  {
    StatusCode sc=tool.retrieve();
    if (sc.isFailure()) ATH_MSG_ERROR("Failed to retrieve correction tool " << tool);
    else ATH_MSG_DEBUG("Successfully retrieved correction tool " << tool);
  }
  return StatusCode::SUCCESS;
}

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
  const xAOD::HIEventShape* eshape = nullptr;
  CHECK(m_modulator_tool->getShape(eshape), 1);

  for(xAOD::CaloClusterContainer::iterator itr=ccl->begin(); itr!=ccl->end(); itr++) 
  {
    xAOD::CaloCluster* cl=*itr;
    xAOD::IParticle::FourMom_t p4;
    if(m_set_moments) m_subtractor_tool->SubtractWithMoments(cl,shape,es_index,m_modulator_tool,eshape);
    else
    {
      m_subtractor_tool->Subtract(p4,cl,shape,es_index,m_modulator_tool,eshape);
      HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedClusterState());
    }
        
  }
  for(ToolHandleArray<CaloClusterCollectionProcessor>::const_iterator toolIt=m_clusterCorrectionTools.begin();
      toolIt!=m_clusterCorrectionTools.end();toolIt++) 
  {
    ATH_MSG_DEBUG(" Applying correction = " << (*toolIt)->name() );
    CHECK((*toolIt)->execute(Gaudi::Hive::currentContext(), ccl), 1);
  }//End loop over correction tools
  return 0;
}

