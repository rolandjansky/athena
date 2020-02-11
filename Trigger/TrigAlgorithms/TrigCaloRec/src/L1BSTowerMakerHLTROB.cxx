/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloRec/L1BSTowerMakerHLTROB.h"

#include "TrigT1CaloByteStream/ITrigT1CaloDataAccessV2.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

// EDM include(s)
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"


L1BSTowerMakerHLTROB::L1BSTowerMakerHLTROB(const std::string &name, ISvcLocator* pSvcLocator): 
  AthAlgorithm(name, pSvcLocator),
  m_tool("LVL1BS__TrigT1CaloDataAccessV2/TrigT1CaloDataAccessV2", 0)
{   
  declareProperty("TrigT1CaloDataAccess", m_tool);
}


StatusCode L1BSTowerMakerHLTROB::initialize()
{
  ATH_MSG_DEBUG("hltInitialize()");

  // retrieve L1Calo tool for reading BS
  if(m_tool.retrieve().isFailure()) {
    
    ATH_MSG_ERROR("couldn't retrieve the tool " << m_tool);
    
    return StatusCode::FAILURE;
  } 
  else {
    
    ATH_MSG_INFO("successfully retrieved tool " << m_tool);
  }
  
  return StatusCode::SUCCESS;
}


L1BSTowerMakerHLTROB::~L1BSTowerMakerHLTROB()
{
  
}


StatusCode L1BSTowerMakerHLTROB::execute()
{
  ATH_MSG_DEBUG("hltExecute()");
  
  TrigRoiDescriptor roi(true);
  
  // prepare xAOD::TriggerTowerContainer (to be filled)
  xAOD::TriggerTowerContainer* container = new xAOD::TriggerTowerContainer();

  xAOD::TriggerTowerAuxContainer* aux_container = new xAOD::TriggerTowerAuxContainer();;
  
  container->setStore(aux_container);
  if ( evtStore()->record(container, "xAODTriggerTowers").isFailure() ) {
    msg() << MSG::ERROR << "Could not record a tower container in the RoI with key AllTowerHLT" << endmsg;
  }
  if ( evtStore()->record(aux_container, "xAODTriggerTowersAux.").isFailure() ) {
    msg() << MSG::ERROR << "Could not record a aux tower container in the RoI with key AllTowerHLTAux." << endmsg;
  }
  
  // use L1Calo tool to fill xAOD::TriggerTowerContainer from BS
  if(m_tool->loadTriggerTowers(*container).isFailure()) {
    
    ATH_MSG_ERROR( "couldn't retrieve the xAOD::TriggerTowers");
    
    return StatusCode::FAILURE;
  }
  else {

    ATH_MSG_DEBUG("successfully retrieved xAOD::TriggerTowers (size = " << container->size() << ")");
    std::cout << "successfully retrieved xAOD::TriggerTowers (size = " << container->size() << ")" << std::endl;
  }
  
  

  return StatusCode::SUCCESS;
}


StatusCode L1BSTowerMakerHLTROB::finalize()
{
  ATH_MSG_DEBUG("hltFinalize()");
  
  return StatusCode::SUCCESS;
}
