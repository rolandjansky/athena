/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Remove objects from temporary background store BkgEvent_2_SG
// before adding new ones
// Piyali.Banerjee@cern.ch February 2011

#include "RemoveTempBkgObjects.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"

#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileL2Container.h"
#include "TileEvent/TileCellIDC.h"
#include "TileEvent/TileBeamElemContainer.h"
#include "TileEvent/TileLaserObject.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArSimEvent/LArHitFloatContainer.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Result/RecRoIBResult.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_RIO.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_RIO.h"
#include "TrigT1CaloEvent/JetElement.h"
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1CaloEvent/JEMEtSums.h"
#include "TrigT1CaloEvent/CPMTower.h" 
#include "TrigT1CaloEvent/RODHeader.h" 
#include "TrigT1CaloEvent/CPMHits.h"
#include "TrigT1CaloEvent/CMMEtSums.h"
#include "TrigT1CaloEvent/JEMRoI.h"
#include "TrigT1CaloEvent/JEMHits.h"
#include "TrigT1CaloEvent/CPMRoI.h"
#include "TrigT1CaloEvent/CMMJetHits.h"
#include "TrigT1CaloEvent/CMMCPHits.h"
#include "TrigT1CaloEvent/CMMRoI.h"

#include <iostream>
#include <typeinfo>

RemoveTempBkgObjects::RemoveTempBkgObjects(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator),
  m_tempBkgStore("StoreGateSvc/BkgEvent_2_SG", name),
  m_tempSigStore("StoreGateSvc/TemporaryStore", name),
  m_bkgData(false)
{
  declareProperty("TempBkgStore", m_tempBkgStore, "help");
  declareProperty("TempSigStore", m_tempSigStore, "help");
  declareProperty("BackgroundIsData", m_bkgData, "help");
}

StatusCode RemoveTempBkgObjects::overlayInitialize()
{

  if (m_tempBkgStore.retrieve().isFailure()) {
     ATH_MSG_FATAL("RemoveTempBkgObjects::initialize(): Temporary Background Store not found !");
     return StatusCode::FAILURE;
  } 
  if (m_tempSigStore.retrieve().isFailure()) {
     ATH_MSG_FATAL("RemoveTempBkgObjects::initialize(): Temporary Signal Store not found !");
     return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

StatusCode RemoveTempBkgObjects::overlayFinalize() 
{
  return StatusCode::SUCCESS;
}

StatusCode RemoveTempBkgObjects::overlayExecute() {
  MsgStream log(msgSvc(), name());
  //std::cout << "RemoveTempBkgObjects::execute() begin"<< std::endl;
  //std::cout << m_storeGateMC->dump() << std::endl;
  //std::cout << m_tempSigStore->dump() << std::endl;
  //std::cout << m_storeGateData->dump() << std::endl;
  //std::cout << m_tempBkgStore->dump() << std::endl;
  //std::cout << m_storeGateOutput->dump() << std::endl;

  checkBeforeRemove<EventInfo>(&*m_tempBkgStore);
  log << MSG::DEBUG << "Removing Eventinfo" << endmsg;

  // If the background is real data and the InDet and Beam objects are
  // not actually present in the temporary background store, which is
  // the case presently, removing them gives warnings and errors 
  // from StoreGate service, so don't remove them
  if (m_bkgData == false) {
     checkBeforeRemove<TRT_RDO_Container>(&*m_tempBkgStore);
     checkBeforeRemove<SCT_RDO_Container>(&*m_tempBkgStore);
     checkBeforeRemove<PixelRDO_Container>(&*m_tempBkgStore);
     log << MSG::DEBUG << "Removing InDet objects" << endmsg;

     checkBeforeRemove<BCM_RDO_Container>(&*m_tempBkgStore);
     log << MSG::DEBUG << "Removing Beam objects" << endmsg;
  }

  checkBeforeRemove<LArFebHeaderContainer>(&*m_tempBkgStore);
  checkBeforeRemove<LArHitFloatContainer>(&*m_tempBkgStore);
  checkBeforeRemove<LArDigitContainer>(&*m_tempBkgStore);
  log << MSG::DEBUG << "Removing LAr objects" << endmsg;

  checkBeforeRemove<TileDigitsContainer>(&*m_tempBkgStore);
  checkBeforeRemove<TileRawChannelContainer>(&*m_tempBkgStore);
  checkBeforeRemove<TileL2Container>(&*m_tempBkgStore);
  checkBeforeRemove<TileCellIDC>(&*m_tempBkgStore);
  checkBeforeRemove<TileBeamElemContainer>(&*m_tempBkgStore);
  checkBeforeRemove<TileLaserObject>(&*m_tempBkgStore);
  log << MSG::DEBUG << "Removing Tile objects" << endmsg;

  checkBeforeRemove<CscDigitContainer>(&*m_tempBkgStore);
  checkBeforeRemove<MdtDigitContainer>(&*m_tempBkgStore);
  checkBeforeRemove<RpcDigitContainer>(&*m_tempBkgStore);
  checkBeforeRemove<TgcDigitContainer>(&*m_tempBkgStore); 
  // If the background is real data and MDTCSM, TGCRDO and RPCPAD objects
  // are not actually present in the temporary background store, removing
  // them gives errors from StoreGate service, so don't remove them
  // Note that because of MDT Cabling problem, these objects are not
  // put into temporary background store if the background is real data
  if (m_bkgData == false) {
     checkBeforeRemove<MdtCsmContainer>(&*m_tempBkgStore);
     checkBeforeRemove<TgcRdoContainer>(&*m_tempBkgStore);
     checkBeforeRemove<RpcPadContainer>(&*m_tempBkgStore);
  }
  checkBeforeRemove<CscRawDataContainer>(&*m_tempBkgStore);
  log << MSG::DEBUG << "Removing Muon objects" << endmsg;

  checkBeforeRemove<ROIB::RoIBResult>(&*m_tempBkgStore);
  checkBeforeRemove<ROIB::RecRoIBResult>(&*m_tempBkgStore);
  checkBeforeRemove<CTP_RDO>(&*m_tempBkgStore);
  checkBeforeRemove<CTP_RIO>(&*m_tempBkgStore);
  checkBeforeRemove<MuCTPI_RDO>(&*m_tempBkgStore);
  checkBeforeRemove<MuCTPI_RIO>(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::JetElement> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::TriggerTower> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::JEMEtSums> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::CPMTower> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::RODHeader> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::CPMHits> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::CMMEtSums> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::JEMRoI> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::JEMHits> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::CPMRoI> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::CMMJetHits> >(&*m_tempBkgStore);
  checkBeforeRemove< DataVector<LVL1::CMMCPHits> >(&*m_tempBkgStore);
  checkBeforeRemove<LVL1::CMMRoI>(&*m_tempBkgStore);
  log << MSG::DEBUG << "Removing Level1 objects" << endmsg;

  //std::cout << "RemoveTempBkgObjects::execute() end"<< std::endl;
  //std::cout << m_storeGateMC->dump() << std::endl;
  //std::cout << m_tempSigStore->dump() << std::endl;
  //std::cout << m_storeGateData->dump() << std::endl;
  //std::cout << m_tempBkgStore->dump() << std::endl;
  //std::cout << m_storeGateOutput->dump() << std::endl;
  return StatusCode::SUCCESS;
}

template<class TypeToBeRemoved>
void RemoveTempBkgObjects::checkBeforeRemove(StoreGateSvc *sg)
{
   const std::string templateClassName = typeid(TypeToBeRemoved).name();

   const DataHandle<TypeToBeRemoved> data;
   const DataHandle<TypeToBeRemoved> dataEnd;
   if (sg->retrieve(data, dataEnd).isSuccess()) {
     ATH_MSG_DEBUG("checkBeforeRemove<"<<templateClassName<<">(): retrieved data = " << data.cptr() << " dataEnd = " << dataEnd.cptr() << " for removal");
     if (data.cptr() != 0) {
        for ( ; data != dataEnd; data++ ) {
           ATH_MSG_DEBUG("checkBeforeRemove<"<<templateClassName<<">(): Working on p="<<data.cptr()<<", key="<<sg->proxy(data.cptr())->name());
           std::string key = sg->proxy(data.cptr())->name();
           if ( sg->contains<TypeToBeRemoved>(key) ) {              
              if (!sg->removeDataAndProxy(data.cptr()).isSuccess()) {
                 ATH_MSG_WARNING("checkBeforeRemove<"<<templateClassName<<">(): problem removing object p="<<data.cptr()<<", key="<<sg->proxy(data.cptr())->name());
              }
           }
        }
     }
   }
}
