/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/TopoClusterVariablesTool.h"

TopoClusterVariablesTool::TopoClusterVariablesTool( const std::string& type,
			      const std::string& name,
			      const IInterface* parent ) : AthAlgTool( type, name, parent ) {
  declareInterface<TopoClusterVariablesTool>( this );

  declareProperty("CaloClusterContainerKey",           _caloClusterContainerKey          = "CaloCalTopoCluster" );

  _dummyClusterCont = new CaloClusterContainer;
}

TopoClusterVariablesTool::~TopoClusterVariablesTool() {
  if (_dummyClusterCont) delete _dummyClusterCont;
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
  sc=evtStore()->retrieve( cluster, _caloClusterContainerKey );
  if( sc.isFailure()  ||  !cluster ) {
    msg() << MSG::WARNING << "TopoClusterVariablesTool: No CaloClusterContainer found in StoreGate, key:" 
	  << _caloClusterContainerKey << endreq; 
    cluster = _dummyClusterCont;
  }
  setCaloClusterContainer(cluster);

  return StatusCode::SUCCESS;
}//end retrieveContainers()
