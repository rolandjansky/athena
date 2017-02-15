/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "ViewAlgs/TrigCompositeTraversal.h"
#include "TestHypoAlgView.h"

namespace AthViews {
TestHypoAlgView::TestHypoAlgView(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthViewAlgorithm(name, pSvcLocator),
    m_hypos(this),
    m_inputProxyContainer("InputProxyContainer"), 
    m_output("Output"), 
    m_outputAux("OutputAux.") 
{
  declareProperty("Hypotheses", m_hypos, "");
  declareProperty("InputProxyContainer", m_inputProxyContainer, "Input ProxyContainer name");
  declareProperty("OutputDecisions", m_output, "Output collection");
  declareProperty("OutputDecisionsAux", m_outputAux, "");
}

StatusCode TestHypoAlgView::initialize() {
  CHECK(m_hypos.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TestHypoAlgView::execute() {

  if ( not m_inputProxyContainer.isValid() ) {
    ATH_MSG_ERROR("Missing Inputs");
    return StatusCode::FAILURE;    
  }
  m_output = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer >();  
  m_output->setStore(m_outputAux.ptr());

#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  size_t nRoI = 0;
  for ( const auto proxy : *m_inputProxyContainer.cptr() ) {
    xAOD::TrigComposite * decision  = new xAOD::TrigComposite();          
    m_output->push_back(decision);   
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
    decision->setObjectLink("seed", ElementLink<xAOD::TrigCompositeContainer>(m_inputProxyContainer.name(), nRoI,eventView(getContext())) ); // set ref. back to the input proxy on which the decision was made
#else
    decision->setObjectLink("seed", ElementLink<xAOD::TrigCompositeContainer>(m_inputProxyContainer.name(), nRoI,eventView(ctx)) ); // set ref. back to the input proxy on which the decision was made
#endif
    
      
      auto objWithCluster = ViewAlgs::find(proxy, ViewAlgs::HasObject("cluster"));
       if ( ! objWithCluster ) {
	ATH_MSG_ERROR("Missing Cluster in this Proxy");
	continue;
      }
      auto cluster = objWithCluster->objectLink<TestClusterContainer>("cluster");      

      std::vector<TriggerElementID> passedHypoIDs;
      for ( const auto& hypoTool : m_hypos ) {
	auto d = hypoTool->decision(*cluster);
	CHECK( d.sc() );	
	ATH_MSG_DEBUG("Hypo Tool " << hypoTool->name() << " decided " << d.content() 
		      << " on cluster Et:" << TestEDM::getClusterEt(*cluster) 
		      << " eta: " << TestEDM::getClusterEta(*cluster) 
		      << " phi: " << TestEDM::getClusterPhi(*cluster));

	if ( d.content() ) 
	  passedHypoIDs.push_back(TrigConf::HLTUtils::string2hash(hypoTool.name(), "TE")); // hypo name is the tag	
      }
      decision->setDetail("Passed", passedHypoIDs);
      nRoI++;
    }




  return StatusCode::SUCCESS;
}

}
