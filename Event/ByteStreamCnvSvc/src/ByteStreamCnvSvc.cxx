/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include "StoreGate/StoreGate.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "eformat/SourceIdentifier.h"
#include "eformat/StreamTag.h"

#include "AthenaKernel/IClassIDSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <algorithm>

/// Standard constructor
ByteStreamCnvSvc::ByteStreamCnvSvc(const std::string& name, ISvcLocator* pSvcLocator)
  : ByteStreamCnvSvcBase(name, pSvcLocator),
    m_evtStore ("StoreGateSvc", name)
{
  declareProperty("ByteStreamOutputSvc",     m_ioSvcName);
  declareProperty("ByteStreamOutputSvcList", m_ioSvcNameList);
  declareProperty("IsSimulation",  m_isSimulation = false);
  declareProperty("IsTestbeam",    m_isTestbeam   = false);
  declareProperty("IsCalibration", m_isCalibration= false);
  declareProperty("GetDetectorMask", m_getDetectorMask = false);
  declareProperty("UserType",      m_userType     = "RawEvent");
  declareProperty("EventStore",    m_evtStore);
}

/// Standard Destructor
ByteStreamCnvSvc::~ByteStreamCnvSvc() {
}

/// Initialize the service.
StatusCode ByteStreamCnvSvc::initialize() {
   if (!ByteStreamCnvSvcBase::initialize().isSuccess()) {
      ATH_MSG_FATAL("ByteStreamCnvSvcBase::initialize() failed");
      return(StatusCode::FAILURE);
   }

   ATH_CHECK( m_evtStore.retrieve() );

   // get ready for output
   std::vector<std::string> ioSvcNames = m_ioSvcNameList.value();
   if (!m_ioSvcName.empty()) {
      // add ioSvcName if ioSvcNameList is missing it
      std::vector<std::string>::iterator it = find(ioSvcNames.begin(), ioSvcNames.end(), m_ioSvcName);
      if (it == ioSvcNames.end()) {
         ioSvcNames.push_back(m_ioSvcName);
      }
   }
   if (ioSvcNames.size() != 0) {
      // Check ByteStreamCnvSvc
      for (std::vector<std::string>::iterator itSvc = ioSvcNames.begin(), itSvcE = ioSvcNames.end();
	      itSvc != itSvcE; ++itSvc) {
         ATH_MSG_DEBUG("get " << *itSvc);
         // get service
         IService* svc;
         if (!service(*itSvc, svc).isSuccess()) {
            ATH_MSG_FATAL("Cannot get ByteStreamOutputSvc");
            return(StatusCode::FAILURE);
         }
         ByteStreamOutputSvc* ioSvc = dynamic_cast<ByteStreamOutputSvc*>(svc);
         if (ioSvc == 0) {
            ATH_MSG_FATAL("Cannot cast to  ByteStreamOutputSvc");
            return(StatusCode::FAILURE);
         }
         // get stream name
         std::string bsOutputStreamName;
         SimpleProperty<std::string> propBSO("BSOutputStreamName", bsOutputStreamName);
         if (!ioSvc->getProperty(&propBSO).isSuccess()) {
            ATH_MSG_FATAL("Cannot get BSOutputStreamName from " << *itSvc);
            return(StatusCode::FAILURE);
         }
         bsOutputStreamName = propBSO.value();
         // append
         m_ioSvcMap[bsOutputStreamName] = ioSvc;
      }
   }
   return(StatusCode::SUCCESS);
}

StatusCode ByteStreamCnvSvc::finalize() {
   return(ByteStreamCnvSvcBase::finalize());
}

StatusCode ByteStreamCnvSvc::connectOutput(const std::string& t, const std::string& /*mode*/) {
   return(connectOutput(t));
}

StatusCode ByteStreamCnvSvc::connectOutput(const std::string& /*t*/) {
   ATH_MSG_DEBUG("In connectOutput");

   // Get the EventInfo obj for run/event number
   const EventInfo* d = nullptr;
   ATH_CHECK( m_evtStore->retrieve(d) );
   uint64_t event = d->event_ID()->event_number();
   uint32_t run_no = d->event_ID()->run_number();
   uint32_t bc_time_sec = d->event_ID()->time_stamp();
   uint32_t bc_time_ns = d->event_ID()->time_stamp_ns_offset();
   uint32_t run_type = 0;
   uint32_t lvl1_id = d->trigger_info()->extendedLevel1ID();
   if (lvl1_id == 0) {
      lvl1_id = event;
   }
   uint32_t lvl1_type = d->trigger_info()->level1TriggerType();
   uint64_t global_id = event;
   uint16_t lumi_block = d->event_ID()->lumi_block();
   uint16_t bc_id = d->event_ID()->bunch_crossing_id();
   static uint8_t nevt = 0;
   nevt = nevt%255;
   // create an empty RawEvent
   eformat::helper::SourceIdentifier sid = eformat::helper::SourceIdentifier(eformat::FULL_SD_EVENT, nevt);
   m_rawEventWrite = new RawEventWrite(sid.code(), bc_time_sec, bc_time_ns, global_id, run_type, run_no, lumi_block, lvl1_id, bc_id, lvl1_type);
   return(StatusCode::SUCCESS);
}

StatusCode ByteStreamCnvSvc::commitOutput(const std::string& outputConnection, bool /*b*/) {
   ATH_MSG_DEBUG("In flushOutput " << outputConnection);

   if (m_ioSvcMap.size() == 0) {
      ATH_MSG_ERROR("ByteStreamCnvSvc not configure for output");
      return(StatusCode::FAILURE);
   }
   writeFEA();

   // Get the EventInfo obj for trigger info
   const EventInfo* evt = nullptr;
   ATH_CHECK( m_evtStore->retrieve(evt) );
   if (evt == 0) {
      ATH_MSG_ERROR("Cannot retrieve EventInfo");
      return(StatusCode::FAILURE);
   }
   // change trigger info in Header
   const TriggerInfo* triggerInfo = evt->trigger_info();
   uint32_t *l1Buff = 0;
   uint32_t *l2Buff = 0;
   uint32_t *efBuff = 0;
   uint32_t *encTag = 0;
   if (triggerInfo != 0) {
      // LVL1 info : FIXME
      if (triggerInfo->extendedLevel1ID() != 0) {
         m_rawEventWrite->lvl1_id(triggerInfo->extendedLevel1ID());
      }
      // trigger type
      m_rawEventWrite->lvl1_trigger_type((uint8_t)(triggerInfo->level1TriggerType()));
      // LVL1 info
      const std::vector<TriggerInfo::number_type> level1TriggerInfo = triggerInfo->level1TriggerInfo();
      uint32_t l1Size = 0;
      l1Buff = new uint32_t[level1TriggerInfo.size()];
      for (std::vector<TriggerInfo::number_type>::const_iterator itL1 = level1TriggerInfo.begin(),
	      itL1E = level1TriggerInfo.end(); itL1 != itL1E; itL1++) {
         l1Buff[l1Size] = *itL1;
         l1Size++;
      }
      m_rawEventWrite->lvl1_trigger_info(l1Size, l1Buff);
      // LVL2 info
      const std::vector<TriggerInfo::number_type> level2TriggerInfo = triggerInfo->level2TriggerInfo();
      uint32_t l2Size = 0;
      l2Buff = new uint32_t[level2TriggerInfo.size()];
      for (std::vector<TriggerInfo::number_type>::const_iterator itL2 = level2TriggerInfo.begin(),
	      itL2E = level2TriggerInfo.end(); itL2 != itL2E; itL2++) {
         l2Buff[l2Size] = *itL2;
         l2Size++;
      }
      m_rawEventWrite->lvl2_trigger_info(l2Size, l2Buff);
      // EF info
      const std::vector<TriggerInfo::number_type> eventFilterInfo = triggerInfo->eventFilterInfo();
      uint32_t efSize = 0;
      efBuff = new uint32_t[eventFilterInfo.size()];
      for (std::vector<TriggerInfo::number_type>::const_iterator itEF  = eventFilterInfo.begin(),
	      itEFE = eventFilterInfo.end(); itEF != itEFE; itEF++) {
         efBuff[efSize] = *itEF;
         efSize++;
      }
      m_rawEventWrite->event_filter_info(efSize, efBuff);
      // stream tag
      std::vector<eformat::helper::StreamTag> on_streamTags;
      const std::vector<TriggerInfo::StreamTag> off_streamTags = triggerInfo->streamTags();
      for (std::vector<TriggerInfo::StreamTag>::const_iterator itS = off_streamTags.begin(),
	      itSE = off_streamTags.end(); itS != itSE; itS++) {
         // convert offline -> online
         eformat::helper::StreamTag tmpTag;
         tmpTag.name = itS->name();
         tmpTag.type = itS->type();
         tmpTag.obeys_lumiblock = itS->obeysLumiblock();
         if (!((itS->robs()).empty())) {
           std::set<TriggerInfo::number_type> tmp_robs = itS->robs();
           for (std::set<TriggerInfo::number_type>::const_iterator it_rob = tmp_robs.begin();
	           it_rob != tmp_robs.end(); it_rob++) {
             tmpTag.robs.insert(*it_rob);
           }
         }
         if (!((itS->dets()).empty())) {
           std::set<TriggerInfo::number_type> tmp_dets = itS->dets();
           for (std::set<TriggerInfo::number_type>::const_iterator it_det = tmp_dets.begin();
	           it_det != tmp_dets.end(); it_det++) {
             tmpTag.dets.insert((eformat::SubDetector) *it_det);
           }
         }
         on_streamTags.push_back(tmpTag);
      }
      // encode
      uint32_t encSize = eformat::helper::size_word(on_streamTags);
      encTag = new uint32_t[encSize];
      eformat::helper::encode(on_streamTags, encSize, encTag);
      m_rawEventWrite->stream_tag(encSize, encTag);
   }
   // convert RawEventWrite to RawEvent
   uint32_t rawSize = m_rawEventWrite->size_word();
   OFFLINE_FRAGMENTS_NAMESPACE::DataType* buffer = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[rawSize];
   uint32_t count = eformat::write::copy(*(m_rawEventWrite->bind()), buffer, rawSize);
   if (count != rawSize) {
      ATH_MSG_ERROR("Memcopy failed");
      return(StatusCode::FAILURE);
   }
   RawEvent rawEvent(buffer);
   // check validity
   try {
      rawEvent.check_tree();
   } catch (...) {
      ATH_MSG_ERROR("commitOutput failed, because FullEventFragment invalid");
      return(StatusCode::FAILURE);
   }
   ATH_MSG_DEBUG("commitOutput: Size of Event (words) = " << rawEvent.fragment_size_word());
   // put event to OutputSvc
   if ((m_ioSvcMap.size() == 1) or (m_ioSvcMap.count(outputConnection) > 0)) {
      std::map<std::string, ByteStreamOutputSvc*>::iterator itSvc = m_ioSvcMap.find(outputConnection);
      // for backward compatibility
      if (itSvc == m_ioSvcMap.end()) {
         itSvc = m_ioSvcMap.begin();
      }
      // put
      if (!itSvc->second->putEvent(&rawEvent)) {
         ATH_MSG_ERROR("commitOutput failed to send output");
         return(StatusCode::FAILURE);
      }
   }
   // delete
   delete [] buffer; buffer = 0;
   delete m_rawEventWrite; m_rawEventWrite = 0;
   delete [] l1Buff; l1Buff = 0;
   delete [] l2Buff; l2Buff = 0;
   delete [] efBuff; efBuff = 0;
   delete [] encTag; encTag = 0;
   // delete FEA
   for (std::map<std::string, FullEventAssemblerBase*>::const_iterator it = m_feaMap.begin(),
	   itE = m_feaMap.end(); it != itE; it++) {
      delete it->second;
   }
   m_feaMap.clear();
   return(StatusCode::SUCCESS);
}

void ByteStreamCnvSvc::writeFEA() {
   ATH_MSG_DEBUG("before FEAMAP size = " << m_feaMap.size());
   for (std::map<std::string, FullEventAssemblerBase*>::const_iterator it = m_feaMap.begin(),
	   itE = m_feaMap.end(); it != itE; it++) {
      MsgStream log(msgSvc(), name());
      (*it).second->fill(m_rawEventWrite, log);
   }
   ATH_MSG_DEBUG("after FEAMAP size = " << m_feaMap.size());
}
