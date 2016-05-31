/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellRec/LArTimeVetoAlg.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArRecEvent/LArEventBitInfo.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "xAODEventInfo/EventInfo.h"

using xAOD::EventInfo;

//Constructor
LArTimeVetoAlg:: LArTimeVetoAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_folderName("/LAR/BadChannelsOfl/EventVeto")
  {
    declareProperty("folderName",m_folderName,"Folder name for DB access");
    m_nevt=0;
    m_nevtMasked=0;
  }
  
//__________________________________________________________________________
//Destructor
  LArTimeVetoAlg::~LArTimeVetoAlg()
  {
    ATH_MSG_DEBUG("LArTimeVetoAlg destructor called");
  }

//__________________________________________________________________________
StatusCode LArTimeVetoAlg::initialize() 
  {
    msg(MSG::INFO)  <<"LArTimeVetoAlg initialize()" << endreq;
    m_nevt=0;
    m_nevtMasked=0;

    StatusCode sc = detStore()->regHandle(m_dd_atrList,m_folderName);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << " cannot register handle to attribute list " << m_folderName << endreq;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS; 

  }

//__________________________________________________________________________
StatusCode LArTimeVetoAlg::finalize()
  {
    msg(MSG::DEBUG) << "LArTimeVetoAlg finalize()" << endreq;
    msg(MSG::INFO) << " Number of events processed " << m_nevt << "   Number of events in LAr bad time interval " << m_nevtMasked << endreq;
    return StatusCode::SUCCESS; 
  }
  
//__________________________________________________________________________
StatusCode LArTimeVetoAlg::execute()
{
  ATH_MSG_DEBUG("LArTimeVetoAlg execute()");

  m_nevt++;

  const uint32_t vetoWord = (*m_dd_atrList)["EventVeto"].data<uint32_t>();

  //std::cout << " vetoWord " << vetoWord << std::endl;

  if (vetoWord != 0) {

    m_nevtMasked++;

    // retrieve EventInfo
    const EventInfo* eventInfo_c=0;
    StatusCode sc = evtStore()->retrieve(eventInfo_c);
    if (sc.isFailure()) {
      msg(MSG::WARNING) << " cannot retrieve EventInfo, will not set LAr bit information " << endreq;
      return StatusCode::SUCCESS;
    }
    EventInfo* eventInfo=0;
    if (eventInfo_c) {
     eventInfo = const_cast<EventInfo*>(eventInfo_c);
    }
    if (eventInfo) {
      if (!eventInfo->setErrorState(EventInfo::LAr,EventInfo::Error)) {
	msg(MSG::WARNING) << " cannot set error state for LAr " << endreq;
      }
      if (vetoWord & 0xFFFF) {
	ATH_MSG_DEBUG("Event flagged as Noise Burst!");
	if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::NOISEBURSTVETO)) {
	  msg(MSG::WARNING) << " cannot set flag bit for LAr " << endreq;
	} 
	
      if (vetoWord & 0x10000) {
	ATH_MSG_DEBUG("Event flagged as Mini-Noise Burst!");
	if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::MININOISEBURSTTIGHT)) {
	  msg(MSG::WARNING) << " cannot set flag bit for LAr " << endreq;
	} 
      }
	
	//std::cout << " after first event flag " << std::hex << eventInfo->eventFlags(EventInfo::LAr) << std::dec << std::endl;
	//Record location (partition) of noise burst
	for (int i=0;i<8;i++) {
	  if (vetoWord & (1<<i)) {
	    if (!eventInfo->setEventFlagBit(EventInfo::LAr,20+i)) {
	      msg(MSG::WARNING) << " cannot set flag bit for LAr documenting noise burst location " << endreq;
	    }
	  }
	}
      }//end if one of the first 16 bits set
      if (vetoWord & 0xFFF80000) {
	ATH_MSG_DEBUG("Event flagged as DataCorruption!");
	if (!eventInfo->setEventFlagBit(EventInfo::LAr,LArEventBitInfo::DATACORRUPTEDVETO)) {
	  msg(MSG::WARNING) << " cannot set flag bit for LAr " << endreq;
	} 
      }//end if one of the second 16 bits set
    }  //if eventInfo

  }   //if vetoWord

  //std::cout << " Lar bit in error word " << eventInfo->errorState(EventInfo::LAr) << std::endl;
  //std::cout << " Lar word              " << std::hex << eventInfo->eventFlags(EventInfo::LAr) << std::dec << std::endl;


  return StatusCode::SUCCESS;
}
