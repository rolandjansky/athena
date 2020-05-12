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

class ISvcLocator;
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
	//From here on temporarily commented out code bc needs a dedicated treatment in MT to compile
	//In rel 21 we were updating a non-const CaloCalusterContainer. That approach is no more acceptable in Athena MT
	//const xAOD::HIEventShapeContainer* shape = 0;
	//SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_eventShapeKey );
  //shape = readHandleEvtShape.get();
  ATH_MSG_WARNING("HIClusterSubtraction not yet migrated to MT. Currently disabled! ");
  //const HIEventShapeIndex* es_index = HIEventShapeMap::getIndex( m_eventShapeKey.key() );

  const xAOD::HIEventShape* eshape = nullptr;
  CHECK(m_modulatorTool->getShape(eshape), 1);

	/*

	SG::ReadHandle<xAOD::CaloClusterContainer>  readHandleClusters ( m_clusterKey );

  if(m_updateMode)
  {
		const xAOD::CaloClusterContainer* ccl = nullptr;
		ccl = readHandleClusters.cptr();
    std::unique_ptr<std::vector<float> > subtractedE(new std::vector<float>());
    subtractedE->reserve(ccl->size());
		//Decoration TODO: check for migration
    SG::AuxElement::Decorator< float > decorator("HISubtractedE");

    for(xAOD::CaloClusterContainer::const_iterator itr=ccl->begin(); itr!=ccl->end(); itr++)
    {
      const xAOD::CaloCluster* cl=*itr;
      xAOD::IParticle::FourMom_t p4;
      m_subtractorTool->subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
      subtractedE->push_back(p4.E());
      decorator(*cl)=p4.E();
    }
  }
  else
  {
		xAOD::CaloClusterContainer* ccl = nullptr;
		ccl = readNonConstHandleCluster.ptr();
    for(xAOD::CaloClusterContainer::iterator itr=ccl->begin(); itr!=ccl->end(); itr++)
    {
      const xAOD::CaloCluster* cl=*itr;
      xAOD::IParticle::FourMom_t p4;
      if(m_setMoments) m_subtractorTool->subtractWithMoments(cl, shape, es_index, m_modulatorTool, eshape);
      else
      {
					m_subtractorTool->subtract(p4,cl,shape,es_index,m_modulatorTool,eshape);
					HIJetRec::setClusterP4(p4,cl,HIJetRec::subtractedClusterState());
      }
    }
    for(ToolHandleArray<CaloClusterCollectionProcessor>::const_iterator toolIt=m_clusterCorrectionTools.begin();
	      toolIt != m_clusterCorrectionTools.end(); toolIt++)
    {
      ATH_MSG_DEBUG(" Applying correction = " << (*toolIt)->name() );
			CHECK((*toolIt)->execute(Gaudi::Hive::currentContext(), ccl), 1);

    }//End loop over correction tools
  }*/
  return 0;
}
