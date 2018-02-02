/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHI/HIEventShapeAugmentationTool.h"
#include "xAODHIEvent/HIEventShapeAuxContainer.h"
#include "HIEventUtils/HIEventShapeSummaryTool.h"


namespace DerivationFramework {
  HIEventShapeAugmentationTool::HIEventShapeAugmentationTool(const std::string& t,
							     const std::string& n,
							     const IInterface* p) : AthAlgTool(t,n,p),
										    m_summary_tool(this)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("InputContainer",m_input_container="HIEventShape");
    declareProperty("OutputContainer",m_output_container="EventShapeCompact");
    declareProperty("SummaryTool",m_summary_tool);
  }


  StatusCode HIEventShapeAugmentationTool::addBranches() const
  {


    const xAOD::HIEventShapeContainer* es=nullptr;
    ATH_CHECK(evtStore()->retrieve(es,m_input_container));
    
    xAOD::HIEventShapeContainer* es_proj=new xAOD::HIEventShapeContainer();
    xAOD::HIEventShapeAuxContainer* es_projAux=new xAOD::HIEventShapeAuxContainer();
    es_proj->setStore(es_projAux);
    
    ATH_CHECK(m_summary_tool->summarize(es,es_proj));
    ATH_MSG_INFO( "addBranches " << m_input_container << "\t" << m_output_container << "\t" << es_proj->size() << "\t" << es->size() );
    

    ATH_CHECK(evtStore()->record(es_proj,m_output_container));
    ATH_CHECK(evtStore()->record(es_projAux,m_output_container+"Aux."));
    return StatusCode::SUCCESS;
  }


}
