/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/TopoClusterVariablesTool.h"

TopoClusterVariablesTool::TopoClusterVariablesTool( const std::string& type,
			      const std::string& name,
			      const IInterface* parent ) : AthAlgTool( type, name, parent ) {
  declareInterface<TopoClusterVariablesTool>( this );

  declareProperty("CaloClusterContainerKey",           m_caloClusterContainerKey          = "CaloCalTopoCluster" );

  m_dummyClusterCont = new CaloClusterContainer;
}

TopoClusterVariablesTool::~TopoClusterVariablesTool() {
  if (m_dummyClusterCont) delete m_dummyClusterCont;
}

StatusCode TopoClusterVariablesTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode TopoClusterVariablesTool::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode TopoClusterVariablesTool::retrieveContainers() {
  StatusCode sc = StatusCode::SUCCESS;

  const CaloClusterContainer *cluster = 0;
  sc=evtStore()->retrieve( cluster, m_caloClusterContainerKey );
  if( sc.isFailure()  ||  !cluster ) {
    msg() << MSG::WARNING << "TopoClusterVariablesTool: No CaloClusterContainer found in StoreGate, key:" 
	  << m_caloClusterContainerKey << endmsg; 
    cluster = m_dummyClusterCont;
  }
  setCaloClusterContainer(cluster);

  return StatusCode::SUCCESS;
}//end retrieveContainers()
