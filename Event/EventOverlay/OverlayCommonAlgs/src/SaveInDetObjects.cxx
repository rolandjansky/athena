/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SaveInDetObjects.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"

#include <iostream>
#include <typeinfo>

//================================================================
SaveInDetObjects::SaveInDetObjects(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator), 
  m_storeGateTemp("StoreGateSvc/BkgEvent_1_SG", name)
{
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty ("SCTType", m_sctType=true);
}

//================================================================
StatusCode SaveInDetObjects::overlayInitialize()
{

  if (m_storeGateTemp.retrieve().isFailure()) {
     ATH_MSG_FATAL("SaveInDetObjects::initialize): StoreGate[temp] service not found !");
     return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode SaveInDetObjects::overlayFinalize() 
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode SaveInDetObjects::overlayExecute() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "SaveInDetObjects::execute() begin"<< endmsg;
  
  copyInDetIDCobject<TRT_RDO_Container,TRT_LoLumRawData>(&*m_storeGateMC,&*m_storeGateTemp);
  if ( m_sctType) copyInDetIDCobject<SCT_RDO_Container,SCT3_RawData>(&*m_storeGateMC,&*m_storeGateTemp);
  else copyInDetIDCobject<SCT_RDO_Container,SCT1_RawData>(&*m_storeGateMC,&*m_storeGateTemp);
  copyInDetIDCobject<PixelRDO_Container,Pixel1RawData>(&*m_storeGateMC,&*m_storeGateTemp);

  log << MSG::DEBUG << "SaveInDetObjects::execute() end"<< endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
