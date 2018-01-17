/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ByteStreamMergeOutputSvc.h"
#include "ByteStreamData/ByteStream.h"

#include "StoreGate/StoreGate.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "eformat/SourceIdentifier.h"

#include <map>

typedef OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment ROBF;
typedef std::map<uint32_t, ROBF*> ROBMAP;

// Constructor.
ByteStreamMergeOutputSvc::ByteStreamMergeOutputSvc(const std::string& name, ISvcLocator* svcloc) :
		ByteStreamOutputSvc(name,svcloc), 
                m_inSvc(0),
                m_outSvc(0)
{
   declareProperty("ByteStreamOutputSvc", m_outSvcName);
   declareProperty("ByteStreamInputSvc", m_inSvcName);
   declareProperty("overWriteHeader", m_overwriteHeader = false);
   declareProperty("BSOutputStreamName", m_bsOutputStreamName = name);
}

// Destructor.
ByteStreamMergeOutputSvc::~ByteStreamMergeOutputSvc() {
}

// setup input and output paths
StatusCode ByteStreamMergeOutputSvc::initialize() {
   StatusCode sc = ByteStreamOutputSvc::initialize();
   if (sc != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Failed to initialize ByteStreamOutputSvc base class");
      return(sc);
   }
   IService* svc;
   sc = service(m_outSvcName.value(), svc);
   if (sc != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Cannot get ByteStreamOutputSvc");
      return(sc);
   }
   m_outSvc = dynamic_cast<ByteStreamOutputSvc*>(svc);
   if (m_outSvc == 0) {
      ATH_MSG_ERROR("Cannot cast " << m_outSvcName << " to ByteStreamOutputSvc");
      return(StatusCode::FAILURE);
   }

   sc = service(m_inSvcName.value(), svc);
   if (sc != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Cannot get ByteStreamInputSvc");
      return(sc);
   }
   m_inSvc = dynamic_cast<ByteStreamInputSvc*>(svc);
   if (m_inSvc == 0) {
      ATH_MSG_ERROR("Cannot cast " << m_inSvcName << " to ByteStreamInputSvc");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

// ROBs from the L2 and event filter needs to be treated specially since the module id
// is used online to identify which node processed an event and it therefore varies from
// event to event - here we remap it to have module id 0, since there should only be one per event
uint32_t ByteStreamMergeOutputSvc::reducedROBid(uint32_t source_id) {
   eformat::helper::SourceIdentifier id = eformat::helper::SourceIdentifier(source_id);
   if (id.subdetector_id() == eformat::TDAQ_LVL2 || id.subdetector_id() == eformat::TDAQ_EVENT_FILTER) {
      return(eformat::helper::SourceIdentifier(id.subdetector_id(), 0).code());
   } else {
      return(source_id);
   }
}

// Read the next event.
bool  ByteStreamMergeOutputSvc::putEvent(RawEvent* newEvent) {
   // get original event
   RawEvent* orgEvent = (RawEvent*)m_inSvc->currentEvent();

   ATH_MSG_DEBUG("original BS size = " << 4 * orgEvent->fragment_size_word());
   ATH_MSG_DEBUG("athena BS size = " << 4 * newEvent->fragment_size_word());

   // do the merge...
   // get all the ROBFragments
   const size_t MAX_ROBFRAGMENTS = 2048;
   OFFLINE_FRAGMENTS_NAMESPACE::PointerType orgRobF[MAX_ROBFRAGMENTS];
   OFFLINE_FRAGMENTS_NAMESPACE::PointerType newRobF[MAX_ROBFRAGMENTS];
   size_t orgrobcount = orgEvent->children(orgRobF, MAX_ROBFRAGMENTS);
   if (orgrobcount == MAX_ROBFRAGMENTS) {
      ATH_MSG_ERROR("ROB buffer overflow");
      return false;
   }
   size_t newrobcount = newEvent->children(newRobF,MAX_ROBFRAGMENTS);
   if (newrobcount == MAX_ROBFRAGMENTS) {
      ATH_MSG_ERROR("ROB buffer overflow");
      return false;
   }

   ROBMAP robsToAdd;
   // loop over all ROBs
   for (size_t irob = 0; irob < orgrobcount; ++irob) {
      ROBF* rob = new ROBF(orgRobF[irob]);
      robsToAdd[reducedROBid(rob->source_id())] = rob;
      ATH_MSG_DEBUG("original ROBFragment, src ID = " << std::hex << rob->source_id());
   }
   // now add/overwrite with newly created robs
   for (size_t irob = 0; irob < newrobcount; ++irob) {
      ROBF* rob = new ROBF(newRobF[irob]);
      ROBMAP::const_iterator it = robsToAdd.find(reducedROBid(rob->source_id()));
      if (it != robsToAdd.end()) {
	 delete it->second;
	 ATH_MSG_DEBUG("overwriting ROBFragment with src ID = " << std::hex << rob->source_id());
      }
      robsToAdd[reducedROBid(rob->source_id())] = rob;
      ATH_MSG_DEBUG("new ROBFragment, src ID = " << std::hex << rob->source_id());
   }
   RawEventWrite* mergedEventWrite = new RawEventWrite();
   // copy header
   RawEvent *event = orgEvent;
   if (m_overwriteHeader) {
      event = newEvent;
   }
   mergedEventWrite->source_id(event->source_id());
   mergedEventWrite->bc_time_seconds(event->bc_time_seconds());
   mergedEventWrite->bc_time_nanoseconds(event->bc_time_nanoseconds());
   mergedEventWrite->global_id(event->global_id());
   mergedEventWrite->run_type(event->run_type());
   mergedEventWrite->run_no(event->run_no());
   mergedEventWrite->lumi_block(event->lumi_block());
   mergedEventWrite->lvl1_id(event->lvl1_id());
   mergedEventWrite->bc_id(event->bc_id());
   mergedEventWrite->lvl1_trigger_type(event->lvl1_trigger_type());
   const OFFLINE_FRAGMENTS_NAMESPACE::DataType *tmp;
   event->status(tmp);
   mergedEventWrite->status(event->nstatus(), tmp);
   event->lvl1_trigger_info(tmp);
   mergedEventWrite->lvl1_trigger_info(event->nlvl1_trigger_info(), tmp);
   event->lvl2_trigger_info(tmp);
   mergedEventWrite->lvl2_trigger_info(event->nlvl2_trigger_info(), tmp);
   event->event_filter_info(tmp);
   mergedEventWrite->event_filter_info(event->nevent_filter_info(), tmp);
   event->stream_tag(tmp);
   mergedEventWrite->stream_tag(event->nstream_tag(), tmp);
   mergedEventWrite->checksum_type(event->checksum_type());
   // copy robs
   for(ROBMAP::iterator it = robsToAdd.begin(), itEnd = robsToAdd.end(); it != itEnd; it++) {
      mergedEventWrite->append(it->second);
   }
   // convert RawEventWrite to RawEvent
   uint32_t rawSize = mergedEventWrite->size_word();
   OFFLINE_FRAGMENTS_NAMESPACE::DataType* buffer = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[rawSize];
   uint32_t count = eformat::write::copy(*(mergedEventWrite->bind()), buffer, rawSize);
   if (count != rawSize) {
      ATH_MSG_ERROR("Memcopy failed " << count << " " << rawSize);
      return false;
   }
   RawEvent newRawEvent(buffer);
   StatusCode sc = m_outSvc->putEvent(&newRawEvent) ? StatusCode::SUCCESS : StatusCode::FAILURE;
   for(ROBMAP::iterator it = robsToAdd.begin(), itEnd = robsToAdd.end(); it != itEnd; it++) {
      delete it->second; it->second = 0;
   }
   delete mergedEventWrite; mergedEventWrite = 0;
   delete [] buffer;
   if (sc != StatusCode::SUCCESS) {
      ATH_MSG_ERROR("Failed to put RawEvent");
   }
   return(true);
}

StatusCode ByteStreamMergeOutputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (ByteStreamOutputSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<ByteStreamOutputSvc*>(this);
  } else {
    // Interface is not directly available: try out a base class
    return(::AthService::queryInterface(riid, ppvInterface));
  }
  addRef();
  return(StatusCode::SUCCESS);
}
