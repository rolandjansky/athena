/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "TrigConfHLTData/HLTUtils.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "ViewAlgs/TrigCompositeTraversal.h"
#include "./TestHypoAlg.h"

TestHypoAlg::TestHypoAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_hypos(this),
    m_clusterProxyContainer("ClusterProxyContainer"), 
    m_output("Output"), 
    m_outputAux("OutputAux.") {
  declareProperty("Hypotheses", m_hypos, "");

  declareProperty("ClusterProxyContainer", m_clusterProxyContainer, "Input ClusterProxyContainer name");
  //  declareProperty("ClusterContainer", m_clusterContainer, "InputClusters");
  declareProperty("OutputDecisions", m_output, "Output collection");
  declareProperty("OutputDecisionsAux", m_outputAux, "");
}

StatusCode TestHypoAlg::initialize() {
  CHECK(m_hypos.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode TestHypoAlg::execute() {
  if ( not m_clusterProxyContainer.isValid() ) {
    ATH_MSG_ERROR("Missing clusters");
    return StatusCode::FAILURE;    
  }
  m_output = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
  m_outputAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer >();  
  m_output->setStore(m_outputAux.ptr());

    size_t nRoI = 0;
    for ( const auto proxy : *m_clusterProxyContainer.cptr() ) {
      xAOD::TrigComposite * decision  = new xAOD::TrigComposite();          
      m_output->push_back(decision);   
      decision->setObjectLink("seed", ElementLink<xAOD::TrigCompositeContainer>(m_clusterProxyContainer.name(), nRoI) ); // set ref. back to the cluster proxy on which the decision was made
      
      auto objWithCluster = ViewAlgs::find(proxy, ViewAlgs::HasObject("cluster"));
      auto cluster = objWithCluster->objectLink<TestClusterContainer>("cluster");      

      std::vector<TriggerElementID> passedHypoIDs;
      for ( const auto& hypoTool : m_hypos ) {
	const bool d = hypoTool->decision(*cluster);
	ATH_MSG_DEBUG("Hypo Tool " << hypoTool->name() << " decided " << d 
		      << " on cluster Et:" << TestEDM::getClusterEt(*cluster) 
		      << " eta: " << TestEDM::getClusterEta(*cluster) 
		      << " phi: " << TestEDM::getClusterPhi(*cluster));

	if ( d ) 
	  passedHypoIDs.push_back(TrigConf::HLTUtils::string2hash(hypoTool.name(), "TE")); // hypo name is the tag	
      }
      decision->setDetail("Passed", passedHypoIDs);
    }




  return StatusCode::SUCCESS;
}
