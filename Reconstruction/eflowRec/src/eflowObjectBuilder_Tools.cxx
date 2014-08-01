/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     eflowObjectBuilder_Tools.cxx
PACKAGE:  offline/Reconstruction/eflowRec

AUTHORS:  M.Hodgkinson
CREATED:  24th January, 2005

********************************************************************/

#include "eflowRec/eflowObjectBuilder_Tools.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowTrack.h"
#include "eflowRec/eflowBaseAlgTool.h"
#include "eflowRec/eflowRecoverSplitShowersTool.h"
#include "eflowRec/eflowLCCalibTool.h"
#include "eflowRec/eflowObjectCreatorTool.h"

#include "FourMomUtils/P4DescendingSorters.h"

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/ListItem.h"

#include <iostream>
using namespace std;


eflowObjectBuilder_Tools::eflowObjectBuilder_Tools(const std::string& name,  ISvcLocator* pSvcLocator):
  eflowBaseAlg(name, pSvcLocator),
  m_eflowClustersOutputName("PFOClusters_JetETMiss"),
  m_eflowCaloObjectsName("eflowCaloObjects01"),
  m_tools(this)
{
  /* The following properties can be specified at run-time (declared in jobOptions file) */
  /* Tool configuration */
  declareProperty( "PrivateToolList",  m_tools, "List of Private AlgTools" );
  /* Name if eflow cluster container to be created */
  declareProperty("EflowClustersOutputName",m_eflowClustersOutputName);
  /* Name of eflow Calo Object to be retrieved */
  declareProperty("EflowCaloObjectsName",m_eflowCaloObjectsName);
}

eflowObjectBuilder_Tools::~eflowObjectBuilder_Tools() { }

StatusCode eflowObjectBuilder_Tools::initialize() {
  
  msg(MSG::DEBUG) << "Initialising eflowObjectBuilder_Tools " << endreq;

  if (service("StoreGateSvc", m_storeGate).isFailure()) {
    msg(MSG::WARNING) << "Unable to retrieve pointer to StoreGateSvc" << endreq;
    return StatusCode::SUCCESS;
  } 

  /* Tool service */
  IToolSvc* myToolSvc;
  if ( service("ToolSvc",myToolSvc).isFailure() ) {
    msg(MSG::WARNING) << " Tool Service Not Found" << endreq;
    return StatusCode::SUCCESS;
  }

  if ( m_tools.retrieve().isFailure() ) {
    msg(MSG::WARNING) << "Failed to retrieve " << m_tools << endreq;
    return StatusCode::SUCCESS;
  } else {
    msg(MSG::INFO) << "Retrieved " << m_tools << endreq;
  }
  
  // print the list of tools - taken from JetRec/JetAlgorithm.cxx
  printTools();

  return StatusCode::SUCCESS;
}

StatusCode eflowObjectBuilder_Tools::execute(){

  msg(MSG::DEBUG) << "Executing eflowObjectBuilder_Tools" << endreq;

  /* Retrieve the eflowCaloObject container, return if not existing */
  const eflowCaloObjectContainer* caloObjectContainer;
  StatusCode sc = m_storeGate->retrieve(caloObjectContainer,m_eflowCaloObjectsName);
  if(sc.isFailure() || !caloObjectContainer ){
    msg(MSG::WARNING) <<" no eflow calo object container" <<endreq; 
    return StatusCode::SUCCESS;
  }

  xAOD::CaloClusterContainer* newCaloClusterContainer = eflowCaloObject::getClusterContainerPtr();
  CHECK( evtStore()->record( newCaloClusterContainer, m_eflowClustersOutputName ) );

  xAOD::CaloClusterAuxContainer* newCaloClusterAuxContainer = eflowCaloObject::getClusterAuxContainerPtr();
  CHECK( evtStore()->record( newCaloClusterAuxContainer, m_eflowClustersOutputName + "Aux." ) );

  /* Run the AlgTools
   * --> CellLevelSubtractionTool, RecoverSplitShowersTool, LCCalibTool, eflowObjectCreatorTool */
  ToolHandleArray< eflowBaseAlgTool >::iterator itAlgTool = m_tools.begin();
  ToolHandleArray< eflowBaseAlgTool >::iterator lastAlgTool  = m_tools.end();
  for (; itAlgTool != lastAlgTool; ++itAlgTool) {
    (*itAlgTool)->execute(const_cast<eflowCaloObjectContainer*>(caloObjectContainer));
  }

  /* Sort clusters by pt TODO: should this be done somewhere else? */
  //Do we need to sort the objects at all?
  //newCaloClusterContainer->sort(P4Sorters::Descending::Pt());

  /* Clear track-cluster links */
  eflowTrackClusterLink::clearInstances();

  return StatusCode::SUCCESS;

}

StatusCode eflowObjectBuilder_Tools::finalize() { return StatusCode::SUCCESS; }

void eflowObjectBuilder_Tools::printTools() {
  // print the list of tools - taken from JetRec/JetAlgorithm.cxx
  msg(MSG::INFO) << " " << endreq;
  msg(MSG::INFO) << "List of tools in execution sequence of eflowObjectBuilder_Tools:" << endreq;
  msg(MSG::INFO) << "------------------------------------" << endreq;
  msg(MSG::INFO) << " " << endreq;
  ToolHandleArray<eflowBaseAlgTool>::iterator itTool = m_tools.begin();
  ToolHandleArray<eflowBaseAlgTool>::iterator lastTool = m_tools.end();
  unsigned int toolCtr = 0;
  for (; itTool != lastTool; itTool++) {
    toolCtr++;
    msg(MSG::INFO) << std::setw(2) << std::setiosflags(std::ios_base::right) << toolCtr << ".) "
    << std::resetiosflags(std::ios_base::right) << std::setw(36) << std::setfill('.')
    << std::setiosflags(std::ios_base::left) << (*itTool)->type() << std::setfill('.')
    << (*itTool)->name() << std::setfill(' ') << endreq;
  }
  msg(MSG::INFO) << " " << endreq;
  msg(MSG::INFO) << "------------------------------------" << endreq;
}
