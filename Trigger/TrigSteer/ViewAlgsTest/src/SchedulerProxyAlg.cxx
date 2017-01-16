/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "CxxUtils/make_unique.h"

#include "./SchedulerProxyAlg.h"

SchedulerProxyAlg::SchedulerProxyAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthViewAlgorithm(name, pSvcLocator),
    m_roisContainer("RegionOfReco"), 
    m_outputClusterContainer("Clusters"), 
    m_outputClusterContainerAux("ClustersAux.")
{  
  declareProperty("RoIsContainer", m_roisContainer, "RegionOfReco");
  declareProperty("OutputClusterContainer", m_outputClusterContainer, "Clusters");
  declareProperty("OutputClusterContainerAux", m_outputClusterContainerAux, "ClustersAux.");
}

StatusCode SchedulerProxyAlg::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode SchedulerProxyAlg::execute() {

  if ( not m_roisContainer.isValid() ) {
    ATH_MSG_ERROR("No decisions object prom previous stage");
    return StatusCode::FAILURE;
  }

  m_outputClusterContainer =  CxxUtils::make_unique<TestClusterContainer >();
  ATH_CHECK(m_outputClusterContainer.commit());

  m_outputClusterContainerAux = CxxUtils::make_unique<TestClusterAuxContainer>();
  ATH_CHECK(m_outputClusterContainerAux.commit());
  m_outputClusterContainer->setStore(m_outputClusterContainerAux.ptr());

  ATH_MSG_INFO( "Launching processing for View with RoIs" );
  for ( const auto roi: *m_roisContainer.cptr() ) {
    ATH_MSG_INFO( " ... " << *roi );
    TestCluster * cl = new TestCluster();
    m_outputClusterContainer->push_back(cl);
    TestEDM::setClusterEt (cl, roi->eta()*10.0); // whatever values
    TestEDM::setClusterEta(cl, roi->eta()+0.01);
    TestEDM::setClusterPhi(cl, roi->phi()-0.05);
  }

  ATH_MSG_INFO( "finished processing views" );
  return StatusCode::SUCCESS;
}

