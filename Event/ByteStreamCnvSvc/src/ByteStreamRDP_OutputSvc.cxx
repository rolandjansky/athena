/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//===================================================================
//  Implementation of ByteStreamRDP_OutputSvc
//  Initial implementation Jan 10, 2003
//             
//===================================================================
//

// Include files.
#include "ByteStreamRDP_OutputSvc.h"
#include "ByteStreamCnvSvcBase/ROBDataProviderSvc.h"

// Constructor.
ByteStreamRDP_OutputSvc::ByteStreamRDP_OutputSvc(const std::string& name, ISvcLocator* svcloc)
	: ByteStreamOutputSvc(name, svcloc) {
      if (m_bsOutputStreamName.empty()) m_bsOutputStreamName = name;
}

// Open the first input file and read the first event.
StatusCode ByteStreamRDP_OutputSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!ByteStreamOutputSvc::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize ByteStreamOutputSvc base class.");
      return(StatusCode::FAILURE);
   }

   // Retrieve ROBDataProviderSvc
   if (!m_robProvider.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ROBDataProviderSvc");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

// Receive the next event without explicit context
bool ByteStreamRDP_OutputSvc::putEvent(RawEvent* re) {
   return putEvent(re, Gaudi::Hive::currentContext());
}

// Receive the next event
bool ByteStreamRDP_OutputSvc::putEvent(RawEvent* re, const EventContext& ctx) {
   EventCache* cache = m_eventsCache.get(ctx);
   cache->releaseEvent();
   const uint32_t reSize = re->fragment_size_word();
   const uint32_t* reStart = re->start();
   cache->dataBuffer = std::make_unique<uint32_t[]>(reSize);
   std::copy(reStart, reStart+reSize, cache->dataBuffer.get());

   // Create a cached RawEvent object from the cached data buffer
   cache->rawEvent = std::make_unique<RawEvent>(cache->dataBuffer.get());

   // Give the RawEvent to ROBDataProvider
   m_robProvider->setNextEvent(ctx, cache->rawEvent.get());

   return true;
}

StatusCode ByteStreamRDP_OutputSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (ByteStreamOutputSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<ByteStreamOutputSvc*>(this);
   } else {
     // Interface is not directly available: try out a base class
     return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
