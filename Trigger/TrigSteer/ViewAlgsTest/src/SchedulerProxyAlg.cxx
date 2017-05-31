/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "CxxUtils/make_unique.h"

#include "./SchedulerProxyAlg.h"

SchedulerProxyAlg::SchedulerProxyAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_roisContainer("RegionOfReco"), 
    m_outputClusterContainer("Clusters")
{  
  declareProperty("RoIsContainer", m_roisContainer, "RegionOfReco");
  declareProperty("OutputClusterContainer", m_outputClusterContainer, "Clusters");

}

StatusCode SchedulerProxyAlg::initialize()
{
  CHECK( m_roisContainer.initialize() );
  CHECK( m_outputClusterContainer.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode SchedulerProxyAlg::execute()
{
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif  

  auto inputHandle = SG::makeHandle( m_roisContainer, ctx );
  if ( not inputHandle.isValid() )  {
    ATH_MSG_ERROR("Input handle is invalid");
    return StatusCode::FAILURE;
  }
  
  
  auto outputClusters = std::make_unique<TestClusterContainer>();
  auto outputClustersAux = std::make_unique<TestClusterAuxContainer>();
  outputClusters->setStore( outputClustersAux.get() );
  
  ATH_MSG_INFO( "Launching processing for View with RoIs" );
  for ( const auto roi: *inputHandle.cptr() )
  {
    ATH_MSG_DEBUG( " ... " << *roi << " adding a cluster at slightly varried position and fake Et");
    TestCluster * cl = new TestCluster();
    outputClusters->push_back( cl );
    TestEDM::setClusterEt (cl, ( roi->eta() + roi->phi() )*10.0); // whatever values
    TestEDM::setClusterEta(cl, roi->eta() + 0.01);
    TestEDM::setClusterPhi(cl, roi->phi() - 0.05);
  }

  auto outputHandle = SG::makeHandle( m_outputClusterContainer );
  CHECK( outputHandle.record( std::move( outputClusters ), std::move( outputClustersAux ) ) );
  ATH_MSG_INFO( "finished processing views" );
  return StatusCode::SUCCESS;
}

