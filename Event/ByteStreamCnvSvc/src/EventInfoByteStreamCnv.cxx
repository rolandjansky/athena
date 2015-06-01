/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoByteStreamCnv.h"
#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamCnvSvc/ByteStreamInputSvc.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ByteStreamMetadata.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "StoreGate/StoreGateSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "eformat/StreamTag.h"

#include <time.h>

// Instantiation of a static factory class used by clients to create
// instances of this service
// static CnvFactory<EventInfoByteStreamCnv> s_factory;
// const  ICnvFactory& EventInfoByteStreamCnvFactory = s_factory;

EventInfoByteStreamCnv::EventInfoByteStreamCnv(ISvcLocator* svcloc)
	: Converter(ByteStream_StorageType, classID(), svcloc),
		m_ByteStreamCnvSvc(0),
		m_robDataProvider("ROBDataProviderSvc", "EventInfoByteStreamCnv"),
		m_mdSvc("InputMetaDataStore", "EventInfoByteStreamCnv"),
                m_isSimulation(false),
                m_isTestbeam(false),
                m_isCalibration(false)
{
}

const CLID& EventInfoByteStreamCnv::classID() {
   return(ClassID_traits<EventInfo>::ID());
}

StatusCode EventInfoByteStreamCnv::initialize() {
  StatusCode sc = Converter::initialize();
  if (StatusCode::SUCCESS != sc) {
    return sc;
  }

  MsgStream log(messageService(), "EventInfoByteStreamCnv");
  log << MSG::DEBUG << " initialize" << endreq;

  // Check ByteStreamCnvSvc
  IService* svc;
  sc = serviceLocator()->getService("ByteStreamCnvSvc", svc);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << " Cant get ByteStreamCnvSvc " << endreq;
    return sc;
  }
  m_ByteStreamCnvSvc = dynamic_cast<ByteStreamCnvSvc*>(svc);
  if (m_ByteStreamCnvSvc == 0) {
    log << MSG::ERROR << " Cant cast to ByteStreamCnvSvc" << endreq;
    return StatusCode::FAILURE;
  }

   if (!m_robDataProvider.retrieve().isSuccess()) {
    log << MSG::ERROR << " Cant get ROBDataProviderSvc" << endreq;
    //  ATH_MSG_FATAL("Cannot get ROBDataProviderSvc");
      return(StatusCode::FAILURE);
   }
   if (!m_mdSvc.retrieve().isSuccess()) {
    log << MSG::ERROR << " Cant get InputMetaDataStore" << endreq;
    //  ATH_MSG_FATAL("Cannot get InputMetaDataStore");
      return(StatusCode::FAILURE);
   }

  // EventType
  SimpleProperty<std::string> propUserType("UserType", m_userType);
  sc = m_ByteStreamCnvSvc->getProperty(&propUserType);
  if (sc.isSuccess()) {
    m_userType = propUserType.value();
    log << MSG::INFO << "UserType : " << m_userType << endreq;
  } else {
    log << MSG::ERROR << "could not get UserType" << endreq;
    return sc;
  }

  SimpleProperty<bool> propIsSimulation("IsSimulation", m_isSimulation);
  sc = m_ByteStreamCnvSvc->getProperty(&propIsSimulation);
  if (sc.isSuccess()) {
    m_isSimulation = propIsSimulation.value();
    log << MSG::INFO << "IsSimulation : " << m_isSimulation << endreq;
  } else {
    log << MSG::ERROR << "could not get IsSimulation" << endreq;
    return sc;
  }

  SimpleProperty<bool> propIsTestbeam("IsTestbeam", m_isTestbeam);
  sc = m_ByteStreamCnvSvc->getProperty(&propIsTestbeam);
  if (sc.isSuccess()) {
    m_isTestbeam = propIsTestbeam.value();
    log << MSG::INFO << "IsTestbeam : " << m_isTestbeam << endreq;
  } else {
    log << MSG::ERROR << "could not get IsTestbeam" << endreq;
    return sc;
  }

  SimpleProperty<bool> propIsCalibration("IsCalibration", m_isCalibration);
  sc = m_ByteStreamCnvSvc->getProperty(&propIsCalibration);
  if (sc.isSuccess()) {
    m_isCalibration = propIsCalibration.value();
    log << MSG::INFO << "IsCalibration : " << m_isCalibration << endreq;
  } else {
    log << MSG::ERROR << "could not get IsCalibration" << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
}

StatusCode EventInfoByteStreamCnv::finalize() {
  MsgStream log(messageService(), "EventInfoByteStreamCnv");
  log << MSG::INFO << "finalize " << endreq;

  StatusCode status = Converter::finalize();
  if (status.isFailure()) {
    log << MSG::WARNING << "Converter::finalize() failed" << endreq;
  }
  return(status);
}

StatusCode EventInfoByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  MsgStream log(messageService(), "EventInfoByteStreamCnv");

  ByteStreamAddress *pRE_Addr;
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  if (!pRE_Addr) {
    log << MSG::ERROR << " Can not cast to ByteStreamAddress " << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << " Creating Objects" << endreq;

  // get RawEvent
  const RawEvent* re = m_robDataProvider->getEvent();
  if (re == 0) {
    log << MSG::ERROR << " Can not get RawEvent " << endreq;
    return StatusCode::FAILURE;
  }

  // Run Number
  int runNumber = re->run_no();

  // Event Number
  uint64_t eventNumber;
  if (re->version() < 0x03010000) {
    eventNumber=re->lvl1_id();
  } else {
    eventNumber=re->global_id();
  }

  // Time Stamp
  uint32_t bc_time_sec = re->bc_time_seconds();
  uint32_t bc_time_ns  = re->bc_time_nanoseconds();
  // bc_time_ns should be lt 1e9.
  if (bc_time_ns > 1000000000) {
    if (runNumber < 20920) {
      //  In M4 we saw otherwise, because sec/ns was swapped in raw data.
      uint32_t temp = bc_time_ns;
      bc_time_ns = bc_time_sec;
      bc_time_sec= temp;
      log << MSG::DEBUG << " bc_time  second/nanosecond swapped, sec/ns = " << bc_time_sec << " " << bc_time_ns << endreq;
    } else { // for later runs, the nanosecond clock sometimes is not reset, making it overrun 1e9.
      // round it off to 1e9
      log << MSG::WARNING << " bc_time nanosecond number larger than 1e9, it is " << bc_time_ns << ", reset it to 1 sec" << endreq;
      bc_time_ns = 1000000000;
    }
  }

  // luminosity block number
  uint16_t lumiBlock = re->lumi_block();

  // bunch crossing identifier
  uint16_t bcID = re->bc_id();

  unsigned int detMask0 = 0xFFFFFFFF, detMask1 = 0xFFFFFFFF, detMask2 = 0xFFFFFFFF, detMask3 = 0xFFFFFFFF;
  // Get ByteStream Metadata from Input MetaData Store
  const ByteStreamMetadata* metadata = 0;
  StatusCode status = m_mdSvc->retrieve(metadata, "ByteStreamMetadata");
  if (!status.isSuccess()) {
    log << MSG::WARNING << "Unable to retrieve Input MetaData for ByteStream" << endreq;
  } else {
    uint64_t detectorMask = metadata->getDetectorMask();
    detMask0 = (unsigned int)(detectorMask & 0x00000000FFFFFFFF);
    detMask1 = (unsigned int)(detectorMask >> 32);
    uint64_t detectorMask2 = metadata->getDetectorMask2();
    detMask2 = (unsigned int)(detectorMask2 & 0x00000000FFFFFFFF);
    detMask3 = (unsigned int)(detectorMask2 >> 32);
  }
  EventID* evtId = new EventID(runNumber, eventNumber, bc_time_sec, bc_time_ns, lumiBlock, bcID, detMask0, detMask1,detMask2,detMask3);

  // Event Type
  EventType*  evtType = new EventType();
  if (m_isSimulation) {
    evtType->add_type(EventType::IS_SIMULATION);
  }
  if (m_isTestbeam) {
    evtType->add_type(EventType::IS_TESTBEAM);
  }
  if (m_isCalibration) {
    evtType->add_type(EventType::IS_CALIBRATION);
  }
  evtType->set_user_type(m_userType);

  // Trigger Info
  const OFFLINE_FRAGMENTS_NAMESPACE::DataType* buffer;
  // status element
  re->status(buffer);
  TriggerInfo::number_type statusElement = *buffer;

  // extended LVL1ID
  TriggerInfo::number_type extendedLevel1ID = re->lvl1_id();

  // LVL1 trigger type
  TriggerInfo::number_type level1TriggerType = re->lvl1_trigger_type();

  // LVL1 trigger info
  std::vector<TriggerInfo::number_type> level1TriggerInfo;
  re->lvl1_trigger_info(buffer);
  for (uint32_t iT1 = 0; iT1 < re->nlvl1_trigger_info(); ++iT1) {
    level1TriggerInfo.push_back(*buffer);
    ++buffer;
  }

  // LVL2 trigger info
  std::vector<TriggerInfo::number_type> level2TriggerInfo;
  re->lvl2_trigger_info(buffer);
  for (uint32_t iT2 = 0; iT2 < re->nlvl2_trigger_info(); ++iT2) {
    level2TriggerInfo.push_back(*buffer);
    ++buffer;
  }

  // EF info
  std::vector<TriggerInfo::number_type> eventFilterInfo;
  re->event_filter_info(buffer);
  for (uint32_t iEF = 0; iEF < re->nevent_filter_info(); ++iEF) {
    eventFilterInfo.push_back(*buffer);
    ++buffer;
  }

  // stream tag
  std::vector<TriggerInfo::StreamTag> streamTags;
  std::vector<eformat::helper::StreamTag> onl_streamTags;
  re->stream_tag(buffer);
  eformat::helper::decode(re->nstream_tag(), buffer, onl_streamTags);
  for (std::vector<eformat::helper::StreamTag>::const_iterator itS = onl_streamTags.begin(),
	  itSE = onl_streamTags.end(); itS != itSE; ++itS) {
    std::set<TriggerInfo::number_type> tmp_off_dets = std::set<TriggerInfo::number_type>();
    if (!((itS->dets).empty())) {
      std::set<eformat::SubDetector> tmp_onl_dets = itS->dets;
      for (std::set<eformat::SubDetector>::const_iterator it_det = tmp_onl_dets.begin();
	      it_det != tmp_onl_dets.end(); ++it_det) {
        tmp_off_dets.insert((TriggerInfo::number_type) *it_det);
      }
    }
    streamTags.push_back(TriggerInfo::StreamTag(itS->name, itS->type, itS->obeys_lumiblock, itS->robs, tmp_off_dets));
  }

  TriggerInfo* trigInfo = new TriggerInfo(statusElement,
	extendedLevel1ID,
	level1TriggerType,
	level1TriggerInfo,
	level2TriggerInfo,
	eventFilterInfo,
	streamTags);

  // record EventInfo
  EventInfo* evtInfo = new EventInfo(evtId, evtType, trigInfo);
  evtInfo->setEventFlags(EventInfo::Core, m_robDataProvider->getEventStatus());
  pObj = StoreGateSvc::asStorable(evtInfo);

  log << MSG::DEBUG << " New EventInfo made, run/event= " << runNumber << " " << eventNumber
      << " Time stamp  = " << ascTime(bc_time_sec) << endreq;

  return StatusCode::SUCCESS;
}

StatusCode EventInfoByteStreamCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) {
  StatusCode sc = StatusCode::SUCCESS;
  MsgStream log(messageService(), "EventInfoByteStreamCnv");
  log << MSG::DEBUG << " Nothing to be done for EventInfo createReps" << endreq;
  return sc;
}

const char* EventInfoByteStreamCnv::ascTime(unsigned int tstamp) {
   struct tm t;
   t.tm_sec   = tstamp;
   t.tm_min   = 0;
   t.tm_hour  = 0;
   t.tm_mday  = 0;
   t.tm_mon   = 0;
   t.tm_year  = 70;
   t.tm_wday  = 00;
   t.tm_yday  = 00;
   t.tm_isdst = 0;
   time_t ct = mktime(&t);
   tm* t2 = gmtime(&ct);
   return(asctime(t2));
}
