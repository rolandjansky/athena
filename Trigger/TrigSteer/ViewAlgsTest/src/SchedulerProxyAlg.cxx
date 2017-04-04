/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "CxxUtils/make_unique.h"

#include "./SchedulerProxyAlg.h"

SchedulerProxyAlg::SchedulerProxyAlg(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_roisContainer("RegionOfReco"), 
    m_outputClusterContainer("Clusters"), 
    m_outputClusterContainerAux("ClustersAux.")
{  
  declareProperty("RoIsContainer", m_roisContainer, "RegionOfReco");
  declareProperty("OutputClusterContainer", m_outputClusterContainer, "Clusters");
  declareProperty("OutputClusterContainerAux", m_outputClusterContainerAux, "ClustersAux.");
}

StatusCode SchedulerProxyAlg::initialize()
{
  CHECK( m_roisContainer.initialize() );
  CHECK( m_outputClusterContainer.initialize() );
  CHECK( m_outputClusterContainerAux.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode SchedulerProxyAlg::execute()
{
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif  

  SG::ReadHandle< ConstDataVector< TrigRoiDescriptorCollection > > inputHandle( m_roisContainer, ctx );
  if ( not inputHandle.isValid() )
  {
    ATH_MSG_ERROR("No decisions object from previous stage");
    return StatusCode::FAILURE;
  }

  SG::WriteHandle< TestClusterContainer > outputHandle( m_outputClusterContainer, ctx );
  outputHandle = CxxUtils::make_unique<TestClusterContainer >();
  ATH_CHECK( outputHandle.commit() );

  SG::WriteHandle< TestClusterAuxContainer > outputHandleAux( m_outputClusterContainerAux, ctx );
  outputHandleAux = CxxUtils::make_unique<TestClusterAuxContainer>();
  ATH_CHECK( outputHandleAux.commit() );
  outputHandle->setStore( outputHandleAux.ptr() );

  ATH_MSG_INFO( "Launching processing for View with RoIs" );
  for ( const auto roi: *inputHandle.cptr() )
  {
    ATH_MSG_INFO( " ... " << *roi );
    TestCluster * cl = new TestCluster();
    outputHandle->push_back(cl);
    TestEDM::setClusterEt (cl, roi->eta()*10.0); // whatever values
    TestEDM::setClusterEta(cl, roi->eta()+0.01);
    TestEDM::setClusterPhi(cl, roi->phi()-0.05);
  }

  ATH_MSG_INFO( "finished processing views" );
  return StatusCode::SUCCESS;
}

