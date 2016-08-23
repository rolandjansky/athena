/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CopyObjects.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "InDetSimData/InDetSimDataCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/CscSimDataCollection.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

#include <iostream>
#include <typeinfo>

//================================================================
CopyObjects::CopyObjects(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator),
  m_storeGateTemp("StoreGateSvc/BkgEvent_1_SG", name)
{
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty ("SCTType", m_sctType=false);
  declareProperty("InDetObjects", m_indet=false);
  declareProperty("MuonObjects", m_muon=false);
  declareProperty("TruthObjects", m_truth=false);
}

//================================================================
StatusCode CopyObjects::overlayInitialize()
{
  if (m_storeGateTemp.retrieve().isFailure()) {
     ATH_MSG_FATAL("SaveInDetObjects::initialize): StoreGate[temp] service not found !");
     return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CopyObjects::overlayFinalize() 
{
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CopyObjects::overlayExecute() {
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "CopyObjects::execute() begin"<< endmsg;
//  std::cout << m_storeGateMC->dump() << std::endl;
//  std::cout << m_storeGateTemp->dump() << std::endl;
//  std::cout << m_storeGateOutput->dump() << std::endl;

  if ( m_indet ) { 
     copyAllObjectsOfType<TRT_RDO_Container>(&*m_storeGateMC,&*m_storeGateTemp);
     copyAllObjectsOfType<SCT_RDO_Container>(&*m_storeGateMC,&*m_storeGateTemp);
     copyAllObjectsOfType<PixelRDO_Container>(&*m_storeGateMC,&*m_storeGateTemp);
  }

  if ( m_muon ) { 
     copyAllObjectsOfType<MdtDigitContainer>(&*m_storeGateMC,&*m_storeGateTemp);
     copyAllObjectsOfType<RpcDigitContainer>(&*m_storeGateMC,&*m_storeGateTemp);
     copyAllObjectsOfType<TgcDigitContainer>(&*m_storeGateMC,&*m_storeGateTemp);
  }

  if ( m_truth ) {
     copyAllObjectsOfType<TrackRecordCollection>(&*m_storeGateOutput, &*m_storeGateMC);
     copyAthenaHitObject<CaloCalibrationHitContainer,CaloCalibrationHit>(&*m_storeGateOutput, &*m_storeGateMC);
     copySDOobject<InDetSimDataCollection,InDetSimData>(&*m_storeGateOutput, &*m_storeGateMC);
     copyMuonSDOobject<MuonSimDataCollection,MuonSimData>(&*m_storeGateOutput, &*m_storeGateMC, "MDT_SDO");
     copyMuonSDOobject<MuonSimDataCollection,MuonSimData>(&*m_storeGateOutput, &*m_storeGateMC, "RPC_SDO");
     copyMuonSDOobject<MuonSimDataCollection,MuonSimData>(&*m_storeGateOutput, &*m_storeGateMC, "TGC_SDO");
     copyMuonSDOobject<CscSimDataCollection,CscSimData>(&*m_storeGateOutput, &*m_storeGateMC, "CSC_SDO");
  }

  log << MSG::DEBUG << "CopyObjects::execute() end"<< endmsg;
//  std::cout << m_storeGateMC->dump() << std::endl;
//  std::cout << m_storeGateTemp->dump() << std::endl;
//  std::cout << m_storeGateOutput->dump() << std::endl;
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
