/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamCnvSvc/ByteStreamCnvSvc.h"
#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"
#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/TrigDecision.h"

#include "eformat/SourceIdentifier.h"
#include "eformat/StreamTag.h"

#include "AthenaKernel/IClassIDSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <algorithm>

namespace {
   /// Convert set<uint32_t> to set<SubDetector>
   std::set<eformat::SubDetector> detsOnline(const std::set<uint32_t>& detsOffline) {
      std::set<eformat::SubDetector> out;
      for (const uint32_t detid : detsOffline)
         out.insert(static_cast<eformat::SubDetector>(detid));
      return out;
   }
}

/// Standard constructor
ByteStreamCnvSvc::ByteStreamCnvSvc(const std::string& name, ISvcLocator* pSvcLocator)
  : ByteStreamCnvSvcBase(name, pSvcLocator),
    m_evtStore ("StoreGateSvc", name)
{
  declareProperty("ByteStreamOutputSvc",     m_ioSvcName);
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

   const EventContext& ctx = Gaudi::Hive::currentContext();
   SlotData& slot = *m_slots.get(ctx);

   // Get the EventInfo obj for run/event number
   const xAOD::EventInfo* evtInfo{nullptr};
   ATH_CHECK( m_evtStore->retrieve(evtInfo) );
   uint64_t event = evtInfo->eventNumber();
   uint32_t run_no = evtInfo->runNumber();
   uint32_t bc_time_sec = evtInfo->timeStamp();
   uint32_t bc_time_ns = evtInfo->timeStampNSOffset();
   uint32_t run_type = 0;
   uint32_t lvl1_id = evtInfo->extendedLevel1ID();
   if (lvl1_id == 0) {
      lvl1_id = event;
   }
   uint8_t lvl1_type = evtInfo->level1TriggerType();
   uint64_t global_id = event;
   uint16_t lumi_block = evtInfo->lumiBlock();
   uint16_t bc_id = evtInfo->bcid();
   uint8_t nevt = 0;
   // create an empty RawEvent
   eformat::helper::SourceIdentifier sid = eformat::helper::SourceIdentifier(eformat::FULL_SD_EVENT, nevt);
   RawEventWrite* re = setRawEvent (std::make_unique<RawEventWrite>(sid.code(), bc_time_sec, bc_time_ns, global_id, run_type, run_no, lumi_block, lvl1_id, bc_id, lvl1_type));

   // set stream tags
   std::vector<eformat::helper::StreamTag> on_streamTags;
   for (const auto& sTag : evtInfo->streamTags()) {
      on_streamTags.emplace_back(sTag.name(), sTag.type(), sTag.obeysLumiblock(), sTag.robs(), detsOnline(sTag.dets()));
   }
   uint32_t nStreamTagWords = eformat::helper::size_word(on_streamTags);
   slot.m_tagBuff.resize (nStreamTagWords);
   eformat::helper::encode(on_streamTags, nStreamTagWords, slot.m_tagBuff.data());
   re->stream_tag(nStreamTagWords, slot.m_tagBuff.data());

   // try to get TrigDecision
   const xAOD::TrigDecision *trigDecision{nullptr};
   if (m_evtStore->retrieve(trigDecision, "xTrigDecision") != StatusCode::SUCCESS) {
      ATH_MSG_DEBUG("Failed to retrieve xAOD::TrigDecision. Leaving empty trigger decision vectors");
      return StatusCode::SUCCESS;
   }

   // LVL1 info
   const std::vector<uint32_t> &tbp = trigDecision->tbp();
   const std::vector<uint32_t> &tap = trigDecision->tap();
   const std::vector<uint32_t> &tav = trigDecision->tav();
   const size_t l1TotSize = tbp.size() + tap.size() + tav.size();
   if (l1TotSize > 0) {
      slot.m_l1Buff.resize (l1TotSize);
      size_t l1Size{0};
      for (const uint32_t tb : tbp) {
         slot.m_l1Buff[l1Size++] = tb;
      }
      for (const uint32_t tb : tap) {
         slot.m_l1Buff[l1Size++] = tb;
      }
      for (const uint32_t tb : tav) {
         slot.m_l1Buff[l1Size++] = tb;
      }
      re->lvl1_trigger_info(l1TotSize, slot.m_l1Buff.data());
   }

   // LVL2 info
   const std::vector<uint32_t>& lvl2PP = trigDecision->lvl2PassedPhysics();
   if (lvl2PP.size() > 0) {
      slot.m_l2Buff = lvl2PP;
      re->lvl2_trigger_info(lvl2PP.size(), slot.m_l2Buff.data());
   }

   // EF info
   const std::vector<uint32_t>& efPP = trigDecision->efPassedPhysics();
   if (efPP.size() > 0) {
      slot.m_efBuff = efPP;
      re->event_filter_info(efPP.size(), slot.m_efBuff.data());
   }

   return(StatusCode::SUCCESS);
}

StatusCode ByteStreamCnvSvc::commitOutput(const std::string& outputConnection, bool /*b*/) {
   ATH_MSG_DEBUG("In flushOutput " << outputConnection);

   const EventContext& ctx = Gaudi::Hive::currentContext();
   SlotData& slot = *m_slots.get(ctx);

   if (m_ioSvcMap.size() == 0) {
      ATH_MSG_ERROR("ByteStreamCnvSvc not configure for output");
      return(StatusCode::FAILURE);
   }

   writeFEA (slot);

   // convert RawEventWrite to RawEvent
   RawEventWrite* re = slot.m_rawEventWrite.get();
   uint32_t rawSize = re->size_word();
   std::vector<uint32_t> buffer (rawSize);
   uint32_t count = eformat::write::copy(*(re->bind()), buffer.data(), rawSize);
   if (count != rawSize) {
      ATH_MSG_ERROR("Memcopy failed");
      return(StatusCode::FAILURE);
   }
   RawEvent rawEvent(buffer.data());
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
   // Clear slot-specific data.
   slot.clear();
   return(StatusCode::SUCCESS);
}

void ByteStreamCnvSvc::writeFEA (SlotData& slot)
{
   FEAMap_t& feaMap = slot.m_feaMap;
   ATH_MSG_DEBUG("before FEAMAP size = " << feaMap.size());
   for (auto& p : feaMap) {
      MsgStream log(msgSvc(), name());
      p.second->fill(slot.m_rawEventWrite.get(), log);
   }
   ATH_MSG_DEBUG("after FEAMAP size = " << feaMap.size());
}


StatusCode ByteStreamCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( interfaceID().versionMatch(riid) )    {
    *ppvInterface = (StoreGateSvc*)this;
  }
  else  {
    // Interface is not directly available: try out a base class
    return ByteStreamCnvSvcBase::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


const InterfaceID& 
ByteStreamCnvSvc::interfaceID() { 
  static const InterfaceID IDByteStreamCnvSvc("ByteStreamCnvSvc", 1, 0);
  return IDByteStreamCnvSvc;
}


RawEventWrite*
ByteStreamCnvSvc::setRawEvent (std::unique_ptr<RawEventWrite> rawEventWrite)
{
  RawEventWrite* ptr = rawEventWrite.get();
  m_slots->m_rawEventWrite = std::move (rawEventWrite);
  return ptr;
}

