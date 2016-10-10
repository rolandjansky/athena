/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "ViewAlgs/TrigCompositeTraversal.h"
#include "ViewAlgs/HypoDecision.h"
#include "./TestHypoAlg.h"

TestHypoAlg::TestHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
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

StatusCode TestHypoAlg::initialize() {
  CHECK(m_hypos.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TestHypoAlg::execute() {
  if ( not m_inputProxyContainer.isValid() ) {
    ATH_MSG_ERROR("Missing Inputs");
    return StatusCode::FAILURE;    
  }
  m_output = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer >();  
  m_output->setStore(m_outputAux.ptr());

  size_t nRoI = 0;
  ATH_MSG_DEBUG("Iterating over the input of size: " << m_inputProxyContainer.cptr()->size());
  for ( const auto proxy : *m_inputProxyContainer.cptr() ) {
    xAOD::TrigComposite * decisionStorage  = new xAOD::TrigComposite();          
    m_output->push_back(decisionStorage);   
    decisionStorage->setObjectLink("seed", ElementLink<xAOD::TrigCompositeContainer>(m_inputProxyContainer.name(), nRoI) ); // set ref. back to the input proxy on which the decision was made
    
    
    auto objWithCluster = ViewAlgs::find(proxy, ViewAlgs::HasObject("cluster"));
    if ( ! objWithCluster ) {
      ATH_MSG_ERROR("Missing Cluster in this Proxy");
      continue;
    }
    auto cluster = objWithCluster->objectLink<TestClusterContainer>("cluster");      
    
    HypoDecision hypoResults;
    //std::vector<TriggerElementID> passedHypoIDs;
    for ( const auto& hypoTool : m_hypos ) {
      auto d = hypoTool->decision(*cluster);
      CHECK( d.sc() );	
      ATH_MSG_DEBUG("Hypo Tool " << hypoTool->name() << " decided " << d.content() 
		    << " on cluster Et:" << TestEDM::getClusterEt(*cluster) 
		    << " eta: " << TestEDM::getClusterEta(*cluster) 
		    << " phi: " << TestEDM::getClusterPhi(*cluster));
      
      hypoResults.addDecision(hypoTool.name(), d.content() );
      //	if ( d.content() ) 
      //	  passedHypoIDs.push_back(TrigConf::HLTUtils::string2hash(hypoTool.name(), "TE")); // hypo name is the tag	
    }
    //decision->setDetail("Passed", passedHypoIDs);
    hypoResults.store(*decisionStorage);
    nRoI++;
  }
  return StatusCode::SUCCESS;
}
