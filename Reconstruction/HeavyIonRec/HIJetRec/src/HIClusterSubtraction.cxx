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
  declareProperty("UpdateOnly", m_updateMode=false);
  
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

  const xAOD::HIEventShape* eshape = nullptr;
  CHECK(m_modulator_tool->getShape(eshape));

  if(m_updateMode)
  {

    const xAOD::CaloClusterContainer* ccl=0;
    if(evtStore()->retrieve(ccl,m_cluster_key).isFailure())
    {
      ATH_MSG_ERROR("Could not retrieve input CaloClusterContainer " << m_cluster_key );
      return 1;
    }
    std::unique_ptr<std::vector<float> > subtractedE(new std::vector<float>());
    subtractedE->reserve(ccl->size());
    SG::AuxElement::Decorator< float > decorator("HISubtractedE");

    for(xAOD::CaloClusterContainer::const_iterator itr=ccl->begin(); itr!=ccl->end(); itr++) 
    {
      const xAOD::CaloCluster* cl=*itr;
      xAOD::IParticle::FourMom_t p4;
      m_subtractor_tool->Subtract(p4,cl,shape,es_index,m_modulator_tool,eshape);
      subtractedE->push_back(p4.E());
      decorator(*cl)=p4.E();
    }
  }
  else
  {
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
      CHECK((*toolIt)->execute(ccl));
      //eventually needs to be changed to following for r21:
      //CHECK((*toolIt)->execute(Gaudi::Hive::currentContext(),ccl));
    }//End loop over correction tools
  }
  return 0;
}

