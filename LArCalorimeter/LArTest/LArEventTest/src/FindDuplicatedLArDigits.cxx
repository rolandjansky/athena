/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArEventTest/FindDuplicatedLArDigits.h"
#include "CaloIdentifier/CaloGain.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBEvent/TBScintillatorCont.h"

#include "LArElecCalib/ILArPedestal.h"
#include <fstream>

 

FindDuplicatedLArDigits::FindDuplicatedLArDigits(const std::string& name, ISvcLocator* pSvcLocator)
  : Algorithm(name, pSvcLocator),
    m_nProblemEvent(0),
    m_storeGateSvc(0),
    m_detStore(0),
    m_onlineHelper(0),
    m_larCablingSvc(0),
    m_emId(0),
    m_hecId(0),
    m_fcalId(0),
    m_log(0)
{ 
  declareProperty("ContainerKey",m_contKey);
  m_nDigits=-1;
}

FindDuplicatedLArDigits::~FindDuplicatedLArDigits() 
{}

StatusCode FindDuplicatedLArDigits::initialize()
{

  m_log=new MsgStream(msgSvc(), name());
  if (!m_log) return StatusCode::FAILURE;

  StatusCode sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure()) 
    {(*m_log) << MSG::ERROR << " Cannot locate StoreGateSvc " << std::endl;
     return StatusCode::FAILURE;
    }

  StoreGateSvc* detStore;
  sc=service("DetectorStore",detStore);
  if (sc!=StatusCode::SUCCESS) {
    (*m_log) << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }
 
  sc= service("DetectorStore",m_detStore);
  if(sc.isFailure()) {
    (*m_log) << MSG::ERROR << "DetectorStore service not found" << endreq;
    return StatusCode::FAILURE;
  }

  //Use CaloIdManager to access detector info
  const CaloIdManager *caloIdMgr=CaloIdManager::instance() ;
  m_emId=caloIdMgr->getEM_ID();
  m_fcalId=caloIdMgr->getFCAL_ID();
  m_hecId=caloIdMgr->getHEC_ID();

  if (!m_emId) {
    (*m_log) << MSG::ERROR << "Could not access lar EM ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (!m_fcalId) {
    (*m_log) << MSG::ERROR << "Could not access lar FCAL ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  if (!m_hecId) {
    (*m_log) << MSG::ERROR << "Could not access lar HEC ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  // get cablingSvc
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    sc = toolSvc->retrieveTool("LArCablingService",m_larCablingSvc);
    if(sc.isFailure()){
      (*m_log) << MSG::ERROR << "Could not retrieve LArCablingService Tool" << endreq;
      return StatusCode::FAILURE;
    }
  } else {
    (*m_log) << MSG::ERROR << "Could not retrieve ToolSvc" << endreq;
    return StatusCode::FAILURE;
  }

  // get onlineHelper
  sc = detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    (*m_log) << MSG::DEBUG << " Found the LArOnlineID helper. " << endreq;
  }

  m_nProblemEvent=0;
  return StatusCode::SUCCESS;
}  

StatusCode FindDuplicatedLArDigits::execute()
{
  // Retrieve EventInfo
  const DataHandle<EventInfo> thisEventInfo;
  StatusCode sc=m_storeGateSvc->retrieve(thisEventInfo);
  unsigned eventnumber=0;
  if (sc!=StatusCode::SUCCESS)
    (*m_log) << MSG::WARNING << "No EventInfo object found!" << endreq;
  else {
    EventID *thisEvent=thisEventInfo->event_ID();
    eventnumber=thisEvent->event_number();
  }

  // Retrieve LArDigitContainer
  const DataHandle < LArDigitContainer > digit_cont;
  if (m_contKey.size())
    sc = m_storeGateSvc->retrieve(digit_cont,m_contKey);
  else
    sc = m_storeGateSvc->retrieve(digit_cont);
  if (sc.isFailure()) 
    {(*m_log) << MSG::ERROR << " Cannot read LArDigitContainer from StoreGate! key=" << m_contKey << endreq;
    return StatusCode::FAILURE;
    }
  (*m_log) << MSG::DEBUG << "Retrieved LArDigitContainer from StoreGate! key=" << m_contKey << endreq;

  /*
  // Retrieve LArFebHeaderContainer
  const LArFebHeaderContainer *larFebHeaderContainer;
  sc= m_storeGateSvc->retrieve(larFebHeaderContainer);
  if (sc.isFailure() || !larFebHeaderContainer) {
    (*m_log) << MSG::DEBUG << "Cannot read LArFebHeaderContainer from StoreGate!" << endreq;
    return StatusCode::FAILURE;
  }
  */
  //const unsigned hashmax=m_onlineHelper->channelHashMax();

  int nDigits=digit_cont->size();
  if (m_nDigits==-1 || m_nDigits==nDigits) { //first event or no change 
    (*m_log) << MSG::DEBUG << "Event " << eventnumber << ": Found " << nDigits << " Digits in container" << endreq;
  }
  else {
    (*m_log) << MSG::ERROR << "Event " << eventnumber << ": Size of digit container changed! Now: " << nDigits << " Previous Event: " << m_nDigits << endreq;
  }
  m_nDigits=nDigits;
  m_bitpattern.reset();
  LArDigitContainer::const_iterator it = digit_cont->begin();
  LArDigitContainer::const_iterator it_e = digit_cont->end();
  unsigned nDoubleDigits=0;
  for(; it!=it_e; ++it){
    const HWIdentifier hwid=(*it)->channelID();//hardwareID();
    const IdentifierHash h=m_onlineHelper->channel_Hash(hwid);
    if (m_bitpattern.test(h)) {
      int barrel_ec = m_onlineHelper->barrel_ec(hwid);
      std::string bc;
      if (barrel_ec==0)
	bc="Barrel";
      else if (barrel_ec==1)
	bc="Endcap";
      else
	bc="unknown";

      int pos_neg   = m_onlineHelper->pos_neg(hwid);
      std::string pn;
      if (pos_neg==0) 
	pn="C-Side";
      else if (pos_neg==1) 
	pn="A-Side";
      else
	pn="unknown";

      int FT        = m_onlineHelper->feedthrough(hwid);
      int slot      = m_onlineHelper->slot(hwid);
      int channel   = m_onlineHelper->channel(hwid);
      (*m_log) << MSG::ERROR << "Event " << eventnumber << " Found duplicated cell! Location: (" << bc << "/" << pn << "/FT=" 
	  << FT << "/Slot=" << slot << "/Channel=" << channel << ")" << endreq;
      nDoubleDigits++;
    }
    m_bitpattern.set(h);
  }
  if (nDoubleDigits) {
    m_nProblemEvent++;
    (*m_log) << MSG::ERROR << "Found " << nDoubleDigits << " duplicated digits in event " << eventnumber << endreq;
    //return StatusCode::RECOVERABLE;
  }
  return StatusCode::SUCCESS;
}// end finalize-method.


StatusCode FindDuplicatedLArDigits::finalize() {
  if (m_nProblemEvent)
    (*m_log) << MSG::ERROR << "Found  " << m_nProblemEvent << " Events with duplicated LArDigits" << endreq;
  else
    (*m_log) << MSG::INFO << "No duplicated LArDigits found." << endreq;
  return StatusCode::SUCCESS;
}
