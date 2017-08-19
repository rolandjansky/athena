/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "ByteStreamNavigationProviderSvc.h"

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddressProviderSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamCnvSvc/IByteStreamSingleEventInputSvc.h"

#include "EventInfo/EventInfo.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/ActiveStoreSvc.h"
#include "SGTools/TransientAddress.h"

#include "PersistentDataModel/DataHeader.h"
//----------------------
// For update address
#include "ByteStreamCnvSvcBase/ByteStreamAddressL1R.h"

//------------------------------------------------------------------------------------
/// Standard constructor
ByteStreamNavigationProviderSvc::ByteStreamNavigationProviderSvc(const std::string& name, ISvcLocator* svc) :
	::AthService(name, svc),
        m_rawEvent(0),
	m_activeStoreSvc("ActiveStoreSvc", name),
	m_addressProvider(0),
	m_singleEventInputSvc("ByteStreamSingleEventInputSvc", name),
	m_robDataProviderSvc("ROBDataProviderSvc", name) {
   declareProperty("TypeNames", m_typeNames);
}

//------------------------------------------------------------------------------------
/// Standard Destructor
ByteStreamNavigationProviderSvc::~ByteStreamNavigationProviderSvc() {}
//------------------------------------------------------------------------------------
/// Initialize the service.
StatusCode ByteStreamNavigationProviderSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   if (!m_singleEventInputSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ByteStreamSingleEventInputSvc");
      return(StatusCode::FAILURE);
   }
   if (!m_robDataProviderSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ROBDataProviderSvc");
      return(StatusCode::FAILURE);
   }
   IService *pIS(0);
   StatusCode status = service("ByteStreamAddressProviderSvc", "ByteStreamAddressProviderSvc", pIS);
   if (status != StatusCode::SUCCESS || 0 == (m_addressProvider = dynamic_cast<IAddressProvider*>(pIS))) {
      ATH_MSG_FATAL("Cannot get ByteStreamAddressProviderSvc");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//------------------------------------------------------------------------------------
/// Finalize the service.
StatusCode ByteStreamNavigationProviderSvc::finalize() {
   if (!m_robDataProviderSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ROBDataProviderSvc");
   }
   return(::AthService::finalize());
}
//------------------------------------------------------------------------------------
StoreGateSvc* ByteStreamNavigationProviderSvc::eventStore() {
   if (m_activeStoreSvc == 0) {
      if (!m_activeStoreSvc.retrieve().isSuccess()) {
         ATH_MSG_ERROR("Cannot get ActiveStoreSvc");
         throw GaudiException("Cannot get ActiveStoreSvc", name(), StatusCode::FAILURE);
      }
   }
   return m_activeStoreSvc->operator->();
}
//------------------------------------------------------------------------------------
StatusCode ByteStreamNavigationProviderSvc::loadAddresses(StoreID::type storeId, tadList& tlist) {
   if (storeId != StoreID::EVENT_STORE) {
      return(StatusCode::SUCCESS);
   }
   // Read bytestream event
   StatusCode sc_re = readEvent();
   if (!sc_re.isSuccess()) {
      if (sc_re.isRecoverable()) {
         // Recoverable error if header(s) was not found. Keep going
         ATH_MSG_DEBUG("loadAddress: Cannot get ByteStream header");
         return(StatusCode::SUCCESS);
      } else {
         ATH_MSG_ERROR("loadAddress: Cannot get ByteStream event");
         return(StatusCode::FAILURE);
      }
   }
   // Set event address for ROBDataProviderService
   m_robDataProviderSvc->setNextEvent(m_rawEvent);
   // Create new transient address from iop
   IOpaqueAddress* iop = new ByteStreamAddress(ClassID_traits<EventInfo>::ID(), "", "");
   SG::TransientAddress* tad = new SG::TransientAddress(ClassID_traits<EventInfo>::ID(), "ByteStreamEventInfo", iop, true);
   tlist.push_back(tad);
   StatusCode sc_pLA = m_addressProvider->preLoadAddresses(storeId, tlist);
   return(sc_pLA);
}
//------------------------------------------------------------------------------------
StatusCode ByteStreamNavigationProviderSvc::updateAddress(StoreID::type storeId,
                                                          SG::TransientAddress* tad,
                                                          const EventContext& ctx) {
   ATH_MSG_DEBUG("In updateAddress for: " << tad->clID() << " / " << tad->name());
   if (storeId != StoreID::EVENT_STORE) {
      return(StatusCode::FAILURE);
   }
   // Check if tad is DataHeader
   if (tad->clID() == ClassID_traits<DataHeader>::ID()) {
      ATH_MSG_DEBUG("pdateAddress: tad is DataHeader");
      return(StatusCode::FAILURE);
   }
   // If there is no Bytestream event, read one in
   if (m_rawEvent == 0) {
      StatusCode sc_re = readEvent();
      // Exit with failure on hard read event error
      if (!sc_re.isSuccess()) {
         ATH_MSG_DEBUG("updateAddress: Cannot get ByteStream event");
         return(StatusCode::FAILURE);
      }
      // Set event address for ROBDataProviderService
      m_robDataProviderSvc->setNextEvent(m_rawEvent);
   }
   // Do Update address like in ByteStreamAddressProviderSvc
   StatusCode sc_dUA = m_addressProvider->updateAddress(storeId, tad, ctx);
   return(sc_dUA);
}
//------------------------------------------------------------------------------------
//
// Read one Bytestream event
// File name and event offset are encoded in a Header token
// with TECH signature 1000
//
StatusCode ByteStreamNavigationProviderSvc::readEvent() {
   const DataHandle<DataHeader> dataHeader, dataHeaderLast;
   // Get all headers
   StatusCode status = eventStore()->retrieve(dataHeader, dataHeaderLast);
   if (!status.isSuccess()) {
      m_rawEvent = 0;
      ATH_MSG_DEBUG("readEvent: Cannot find any DataHeader in StoreGate");
      return(StatusCode::RECOVERABLE);
   }
   std::string file;
   const Token* token = 0;
   // Find a header which has a token with Bytestrean attributes (TECH=1000)
   for (; dataHeader != dataHeaderLast; dataHeader++) {
      if (dataHeader->begin()->getToken() != 0
	      && dataHeader->begin()->getToken()->technology() == 0x00001000) {
          // Key hold file name
         file  = dataHeader->getProcessTag();
         token = dataHeader->begin()->getToken();
      } else if (token == 0 && dataHeader->checkStatus(DataHeader::Primary)) {
         for (std::vector<DataHeaderElement>::const_iterator i = dataHeader->beginProvenance(),
		         iEnd = dataHeader->endProvenance(); i != iEnd; i++) {
            if (i->getToken() != 0 && i->getToken()->technology() == 0x00001000) {
               token = i->getToken();
            }
         }
      }
   }
   // If there is no such header then return
   if (file.empty()) {
      if (token == 0) {
         //set pointer to BS event to null
         m_rawEvent = 0;
         ATH_MSG_DEBUG("readEvent: Cannot find BS DataHeader in StoreGate");
         return(StatusCode::RECOVERABLE);
      } else {
         StatusCode status = eventStore()->retrieve(dataHeader, token->toString());
         if (!status.isSuccess()) {
            m_rawEvent = 0;
            ATH_MSG_DEBUG("readEvent: Cannot retrieve BS DataHeader from StoreGate");
            return(StatusCode::RECOVERABLE);
         }
         file  = dataHeader->getProcessTag();
         token = dataHeader->begin()->getToken();
      }
   }
   if (token == 0) {
      ATH_MSG_ERROR("readEvent: Cannot get RawEvent Token");
      return(StatusCode::FAILURE);
   }
   // Get event offset from the token
   long long int offset = token->oid().second; 
   if (!token->contID().empty()) {
      unsigned long long int cntID;
      sscanf(token->contID().c_str(), "%08llX", &cntID);
      offset += (long long int)(cntID<<32);
   }
   ATH_MSG_DEBUG("readEvent: File name    : " << file);
   ATH_MSG_DEBUG("readEvent: Ev. Offset   : " << offset);
   // read one event and return pointer to it
   m_rawEvent = m_singleEventInputSvc->getEvent(file, offset);
   // Check if something is wrong
   if (m_rawEvent == 0) {
      ATH_MSG_ERROR("readEvent: Cannot retrieve RawEvent");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
