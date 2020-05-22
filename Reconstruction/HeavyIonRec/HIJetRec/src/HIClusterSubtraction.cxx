/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIClusterSubtraction.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIJetRec/HIJetRecDefs.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

//**********************************************************************

HIClusterSubtraction::HIClusterSubtraction(std::string name) : asg::AsgTool(name)//,
{
}

//**********************************************************************

StatusCode HIClusterSubtraction::initialize()
{
	//Keys initialization
	ATH_CHECK( m_eventShapeKey.initialize() );
	ATH_CHECK( m_clusterKey.initialize() );

  for (auto tool :  m_clusterCorrectionTools)
  {
    StatusCode sc = tool.retrieve();
    if (sc.isFailure()) ATH_MSG_ERROR("Failed to retrieve correction tool " << tool);
    else ATH_MSG_DEBUG("Successfully retrieved correction tool " << tool);
  }
  return StatusCode::SUCCESS;
}

int HIClusterSubtraction::execute() const
{

  //const jet::cellset_t & badcells = badCellMap.cells() ;
  //retrieve UE
	//From here on temporarily commented out code bc decorators needs a dedicated treatment in MT
	//const xAOD::HIEventShapeContainer* shape = 0;
	SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_eventShapeKey );
  //shape = readHandleEvtShape.get();
  ATH_MSG_WARNING("HIClusterSubtraction not yet migrated to MT. Currently disabled! ");
  //const HIEventShapeIndex* es_index = HIEventShapeMap::getIndex( m_eventShapeKey.key() );

  const xAOD::HIEventShape* eshape = nullptr;
  //Hibryd merging between commit c2aeaed0 to master and 5af8a733 to 21.0ss
  CHECK(m_modulatorTool->getShape(eshape), 1);
	//This part regards decoration!
	//Needs a more deep implementation to work in MT - to be discussed w/ Bill Balunas
	//Will be fronted in the next step of the migration
  /*
	SG::ReadHandle<xAOD::CaloClusterContainer>  read_handle_clusters ( m_clusterKey );
	if (!read_handle_clusters.isValid())
	{
		ATH_MSG_ERROR("Could not retrieve input CaloClusterContainer " << m_clusterKey.key() );
		return 1;
	}
	const xAOD::CaloClusterContainer* ccl=0;
  if(m_updateMode)
  {
		ccl = read_handle_clusters.get();
    //if(evtStore()->retrieve(ccl,m_clusterKey).isFailure())
    std::unique_ptr<std::vector<float> > subtractedE(new std::vector<float>());
    subtractedE->reserve(ccl->size());
		//Decoration TODO: check for migration
    SG::AuxElement::Decorator< float > decorator("HISubtractedE");

    for(xAOD::CaloClusterContainer::const_iterator itr=ccl->begin(); itr!=ccl->end(); itr++)
    {
      const xAOD::CaloCluster* cl=*itr;
      xAOD::IParticle::FourMom_t p4;
      m_subtractorTool->Subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
      subtractedE->push_back(p4.E());
      decorator(*cl)=p4.E();
    }
  }
  else
  {
		xAOD::CaloClusterContainer* ccl=0;
		ccl = read_handle_clusters.get();
    for(xAOD::CaloClusterContainer::iterator itr=ccl->begin(); itr!=ccl->end(); itr++)
    {
      xAOD::CaloCluster* cl=*itr;
      xAOD::IParticle::FourMom_t p4;
      if(m_setMoments) m_subtractorTool->SubtractWithMoments(cl, shape, es_index, m_modulatorTool, eshape);
      else
      {
					m_subtractorTool->Subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
					HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedClusterState());
      }
    }
    for(ToolHandleArray<CaloClusterCollectionProcessor>::const_iterator toolIt=m_clusterCorrectionTools.begin();
	      toolIt != m_clusterCorrectionTools.end(); toolIt++)
    {
      ATH_MSG_DEBUG(" Applying correction = " << (*toolIt)->name() );
			CHECK((*toolIt)->execute(Gaudi::Hive::currentContext(), ccl), 1);
			//Hibryd merging between commit c2aeaed0 to master and 5af8a733 to 21.0
			//eventually needs to be changed to following for r21:
			//CHECK((*toolIt)->execute(ccl), 1);
    }//End loop over correction tools
  }*/
  return 0;
}
