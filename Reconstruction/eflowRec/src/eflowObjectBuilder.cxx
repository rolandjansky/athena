/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowObjectBuilder.cxx
 *
 *  Created on: Mar 2, 2015
 *      Author: zhangrui
 */

#include "eflowRec/eflowObjectBuilder.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowBaseAlgTool.h"
#include "eflowRec/eflowLCCalibTool.h"
#include "eflowRec/eflowObjectCreatorTool.h"

#include "FourMomUtils/P4DescendingSorters.h"

#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/ListItem.h"

#include <iostream>
using namespace std;


eflowObjectBuilder::eflowObjectBuilder(const std::string& name,  ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator),
  m_eflowClustersOutputName("PFOClusters_JetETMiss"),
  m_eflowCaloObjectsName("eflowCaloObjects01"),
  m_storeGate(nullptr),
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

eflowObjectBuilder::~eflowObjectBuilder() { }

StatusCode eflowObjectBuilder::initialize() {

  ATH_MSG_DEBUG("Initialising eflowObjectBuilder ");

  if (service("StoreGateSvc", m_storeGate).isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve pointer to StoreGateSvc");
    return StatusCode::SUCCESS;
  }

  /* Tool service */
  IToolSvc* myToolSvc;
  if ( service("ToolSvc",myToolSvc).isFailure() ) {
    ATH_MSG_WARNING(" Tool Service Not Found");
    return StatusCode::SUCCESS;
  }

  if ( m_tools.retrieve().isFailure() ) {
    ATH_MSG_WARNING("Failed to retrieve " << m_tools);
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_VERBOSE("Retrieved " << m_tools);
  }

  // print the list of tools - taken from JetRec/JetAlgorithm.cxx
  printTools();

  return StatusCode::SUCCESS;
}

StatusCode eflowObjectBuilder::execute(){

  msg(MSG::DEBUG) << "Executing eflowObjectBuilder" << endmsg;
  StatusCode sc;

  /* Retrieve the eflowCaloObject container, return if not existing */
  const eflowCaloObjectContainer* caloObjectContainer;
  sc = m_storeGate->retrieve(caloObjectContainer,m_eflowCaloObjectsName);
  if(sc.isFailure() || !caloObjectContainer ){
    msg(MSG::WARNING) <<" no eflow calo object container" <<endmsg;
    return StatusCode::SUCCESS;
  }

  xAOD::CaloClusterContainer* newCaloClusterContainer = eflowCaloObject::getClusterContainerPtr();
  CHECK( evtStore()->record( newCaloClusterContainer, m_eflowClustersOutputName ) );

  xAOD::CaloClusterAuxContainer* newCaloClusterAuxContainer = eflowCaloObject::getClusterAuxContainerPtr();
  CHECK( evtStore()->record( newCaloClusterAuxContainer, m_eflowClustersOutputName + "Aux." ) );

  /* Run the AlgTools
   * --> MomentCalculator, LCCalibTool, ObjectCreatorTool */
  ToolHandleArray< eflowBaseAlgTool >::iterator itAlgTool = m_tools.begin();
  ToolHandleArray< eflowBaseAlgTool >::iterator lastAlgTool  = m_tools.end();
  for (; itAlgTool != lastAlgTool; ++itAlgTool) {
    (*itAlgTool)->execute(const_cast<eflowCaloObjectContainer*>(caloObjectContainer));
  }

  return StatusCode::SUCCESS;

}

StatusCode eflowObjectBuilder::finalize() { return StatusCode::SUCCESS; }

void eflowObjectBuilder::printTools() {
  // print the list of tools - taken from JetRec/JetAlgorithm.cxx
  ATH_MSG_VERBOSE(" ");
  ATH_MSG_VERBOSE("List of tools in execution sequence of eflowObjectBuilder:");
  ATH_MSG_VERBOSE("------------------------------------");
  ATH_MSG_VERBOSE(" ");
  ToolHandleArray<eflowBaseAlgTool>::iterator itTool = m_tools.begin();
  ToolHandleArray<eflowBaseAlgTool>::iterator lastTool = m_tools.end();
  unsigned int toolCtr = 0;
  for (; itTool != lastTool; itTool++) {
    toolCtr++;
    ATH_MSG_VERBOSE(std::setw(2) << std::setiosflags(std::ios_base::right) << toolCtr << ".) "
    << std::resetiosflags(std::ios_base::right) << std::setw(36) << std::setfill('.')
    << std::setiosflags(std::ios_base::left) << (*itTool)->type() << std::setfill('.')
		    << (*itTool)->name() << std::setfill(' '));
  }
  ATH_MSG_VERBOSE(" ");
  ATH_MSG_VERBOSE("------------------------------------");
}
