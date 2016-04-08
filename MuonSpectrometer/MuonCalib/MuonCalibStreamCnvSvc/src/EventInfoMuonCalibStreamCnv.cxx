/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStreamCnvSvc/EventInfoMuonCalibStreamCnv.h"
#include "MuonCalibStreamCnvSvc/MuonCalibStreamCnvSvc.h" 
#include "MuonCalibStreamCnvSvc/MuonCalibStreamAddress.h" 
#include "MuonCalibStreamCnvSvc/MuonCalibStreamDataProviderSvc.h" 
#include "MuonCalibStreamCnvSvc/MuonCalibStreamInputSvc.h" 

#include "MuCalDecode/CalibEvent.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"

#include "StoreGate/StoreGateSvc.h"  

#include <time.h> 
#include <iostream> 

// Instantiation of a static factory class used by clients to create
// instances of this service 
// static CnvFactory<EventInfoMuonCalibStreamCnv> s_factory;
// const  ICnvFactory& EventInfoMuonCalibStreamCnvFactory = s_factory;

EventInfoMuonCalibStreamCnv::EventInfoMuonCalibStreamCnv(ISvcLocator* svcloc)
  : Converter(MuonCalibStream_StorageType, classID() , svcloc),m_MuonCalibStreamCnvSvc(0),m_dataProvider(0) {
}

const CLID& EventInfoMuonCalibStreamCnv::classID() { 
  return ClassID_traits<EventInfo>::ID();
}

StatusCode EventInfoMuonCalibStreamCnv::initialize() {
  StatusCode sc = Converter::initialize(); 
  if(StatusCode::SUCCESS!=sc) { 
    return sc; 
  } 

  MsgStream log(messageService(), "EventInfoMuonCalibStreamCnv");
  log << MSG::DEBUG<< " initialize EventInfoMuonCalibStreamCnv" <<endreq; 

  // Check MuonCalibStreamCnvSvc
  IService* svc;
  sc = serviceLocator()->getService("MuonCalibStreamCnvSvc", svc); 
  if(sc != StatusCode::SUCCESS ) {
    log<<MSG::ERROR << " Cannot get MuonCalibStreamCnvSvc " <<endreq;
    return sc;
  }

  m_MuonCalibStreamCnvSvc = dynamic_cast<MuonCalibStreamCnvSvc*> (svc); 
  if(m_MuonCalibStreamCnvSvc == 0 ) {
    log<<MSG::ERROR<< "  Cannot cast to  MuonCalibStreamCnvSvc " <<endreq; 
    return StatusCode::FAILURE;
  } 
   
  sc = serviceLocator()->getService("MuonCalibStreamDataProviderSvc", svc); 
  if(sc != StatusCode::SUCCESS ) {
    log<<MSG::ERROR << " Cannot get MuonCalibStreamDataProviderSvc " <<endreq;
    return sc;
  }

  m_dataProvider = dynamic_cast<MuonCalibStreamDataProviderSvc*> (svc); 
  if(m_dataProvider == 0 ) {
    log<<MSG::ERROR<< " Cannot cast to MuonCalibStreamDataProviderSvc " <<endreq; 
    return StatusCode::FAILURE;
  } 
  return StatusCode::SUCCESS; 
}

StatusCode EventInfoMuonCalibStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  MsgStream log(messageService(), "EventInfoMuonCalibStreamCnv");
  static unsigned int timeStamp_previous(0);    //save previous timeStamp to use in case timeStamp=0

  MuonCalibStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<MuonCalibStreamAddress*>(pAddr); 
  if(!pRE_Addr) {
    log << MSG::ERROR << " Cannot cast to MuonCalibStreamAddress " << endreq; 
    return StatusCode::FAILURE;    
  }
  
  log << MSG::DEBUG<<" Creating Objects   " <<endreq; 

// get CalibEvent
  const LVL2_MUON_CALIBRATION::CalibEvent* re = m_dataProvider->getEvent();
  if (!re) {
    log << MSG::ERROR << " Can not get CalibEvent " << endreq; 
    return StatusCode::FAILURE;    
  }

// Event Type
  EventType* peT = new EventType();
  peT->add_type(EventType::IS_CALIBRATION);
  peT->set_user_type("IS_CALIBRATION");

  // Run Number
  // for the time being we handle the run number via the dataProvider
  //  int runNumber = re->run_number();
  int runNumber = m_dataProvider->fakeRunN(); 

  // Event Number
  // for the time being we handle the event number via the dataProvider
  //  int eventNumber = re->lvl1_id(); 
  int eventNumber = m_dataProvider->fakeEventN();; 

  // Time Stamp
  unsigned int timeStamp = re->timestamp();
  // 2105-05-20 (temporary?) fix for timeStamps which are zero.
  if( timeStamp == 0 ) {
    timeStamp = timeStamp_previous;
    log << MSG::INFO << " TIMESTAMP ZERO, run/event= "
        << runNumber << " " << eventNumber << " timestamp=" << timeStamp 
	<< " previous timestamp=" << timeStamp_previous 
        << " previous Date = " << ascTime(timeStamp_previous) << endreq; 
  } else {
    timeStamp_previous = timeStamp;
  }
  int lumiBlock = m_dataProvider->fakeLumiBlock();

  //PRINT TIMESTAMP INFO 
  //std::cout << " New EventInfo made, run/event= "
  // << runNumber << " " << eventNumber << " " << timeStamp 
  //  << " " << ascTime(timeStamp) << std::endl; 

  pObj = StoreGateSvc::asStorable (new EventInfo( new EventID(runNumber,eventNumber,timeStamp, 0, lumiBlock, 0),peT));
  
  log << MSG::DEBUG << " New EventInfo made, run/event= "
      << runNumber << " " << eventNumber << " timestamp=" << timeStamp 
      << " Date = " << ascTime(timeStamp) << endreq; 
 
  return StatusCode::SUCCESS;  
}

// Fill RawEvent object from DataObject
// (but really just configure the message service) 
StatusCode EventInfoMuonCalibStreamCnv::createRep(DataObject* /* pObj */, 
		IOpaqueAddress*& /* pAddr */ )  {

  StatusCode sc = StatusCode::SUCCESS; 
  MsgStream log(messageService(), "EventInfoMuonCalibStreamCnv");

  log <<MSG::DEBUG<< " Nothing to be done for EventInfo createReps " << endreq; 
  return sc; 
}

// Convert (unix) timestamp to a (human-readable) calendar date
const char* EventInfoMuonCalibStreamCnv::ascTime( unsigned int tstamp) {     
  struct tm t;

  t.tm_sec = tstamp;
  t.tm_min= 0;
  t.tm_hour=0;
  t.tm_mday=0;
  t.tm_mon =0;
  t.tm_year=70;  
  t.tm_wday=00;
  t.tm_yday=00;
  t.tm_isdst=0;

  time_t ct = mktime(&t);
  tm* t2 = gmtime(&ct);

  return  asctime(t2);
} 
