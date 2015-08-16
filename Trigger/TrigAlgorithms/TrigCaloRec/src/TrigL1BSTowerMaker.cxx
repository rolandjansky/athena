/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloRec/TrigL1BSTowerMaker.h"

#include "TrigT1CaloByteStream/ITrigT1CaloDataAccessV2.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"


TrigL1BSTowerMaker::TrigL1BSTowerMaker(const std::string &name, ISvcLocator* pSvcLocator): 
  HLT::AllTEAlgo(name, pSvcLocator),
  m_tool("LVL1BS__TrigT1CaloDataAccessV2/TrigT1CaloDataAccessV2", 0)
{   
  declareProperty("TrigT1CaloDataAccess", m_tool);
}


HLT::ErrorCode TrigL1BSTowerMaker::hltInitialize()
{
  ATH_MSG_DEBUG("hltInitialize()");

  // retrieve L1Calo tool for reading BS
  if(m_tool.retrieve().isFailure()) {
    
    ATH_MSG_ERROR("couldn't retrieve the tool " << m_tool);
    
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::BAD_JOB_SETUP);
  } 
  else {
    
    ATH_MSG_INFO("successfully retrieved tool " << m_tool);
  }
  
  return HLT::OK;
}


TrigL1BSTowerMaker::~TrigL1BSTowerMaker()
{
  
}


HLT::ErrorCode TrigL1BSTowerMaker::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& /*tes_in*/, unsigned int type_out)
{
  ATH_MSG_DEBUG("hltExecute()");
  
  // prepare an outputTE
  TrigRoiDescriptor* roi = new TrigRoiDescriptor(true);
  
  HLT::TriggerElement* outputTE = addRoI(type_out,roi);

  outputTE->setActiveState(true); // ?
  
  // prepare xAOD::TriggerTowerContainer (to be filled)
  xAOD::TriggerTowerContainer* container = new xAOD::TriggerTowerContainer();

  xAOD::TriggerTowerAuxContainer aux_container;
  
  container->setStore(&aux_container);
  
  // use L1Calo tool to fill xAOD::TriggerTowerContainer from BS
  if(m_tool->loadTriggerTowers(*container).isFailure()) {
    
    ATH_MSG_ERROR( "couldn't retrieve the xAOD::TriggerTowers");
    
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,HLT::Reason::MISSING_FEATURE);
  }
  else {

    ATH_MSG_DEBUG("successfully retrieved xAOD::TriggerTowers (size = " << container->size() << ")");
  }
  
  // attach to outputTE
  std::string key = "";
  
  HLT::ErrorCode HLT_EC = attachFeature(outputTE,container,key);

  return HLT_EC;
}


HLT::ErrorCode TrigL1BSTowerMaker::hltFinalize()
{
  ATH_MSG_DEBUG("hltFinalize()");
  
  return HLT::OK;
}
