/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolAddressProviderSvc.cxx
 *  @brief This file contains the implementation for the AthenaPoolAddressProviderSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolAddressProviderSvc.h"
#include "registerKeys.h"

#include "PersistentDataModel/AthenaAttributeList.h"
#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModel/TokenAddress.h"

// Framework
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include "StoreGate/ActiveStoreSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IClassIDSvc.h"

#include <vector>

//________________________________________________________________________________
AthenaPoolAddressProviderSvc::AthenaPoolAddressProviderSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthService(name, pSvcLocator),
	m_activeStoreSvc("ActiveStoreSvc", name),
	m_clidSvc("ClassIDSvc", name),
	m_guid() {
   declareProperty("BackNavigation",      m_backNavigationFlag = false);
   declareProperty("BackNavigationScope", m_backNavigationScope);
   declareProperty("DataHeaderKey",       m_dataHeaderKey = "EventSelector");
   declareProperty("DataHeaderIterator",  m_dataHeaderIterator = false);
}
//________________________________________________________________________________
AthenaPoolAddressProviderSvc::~AthenaPoolAddressProviderSvc() {
}
//________________________________________________________________________________
StoreGateSvc* AthenaPoolAddressProviderSvc::eventStore() const {
   if (m_activeStoreSvc == 0) {
      if (!m_activeStoreSvc.retrieve().isSuccess()) {
         ATH_MSG_ERROR("Cannot get ActiveStoreSvc");
         throw GaudiException("Cannot get ActiveStoreSvc", name(), StatusCode::FAILURE);
      }
   }
   return(m_activeStoreSvc->operator->());
}
//________________________________________________________________________________
StatusCode AthenaPoolAddressProviderSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }
   // Retrieve ClassIDSvc
   if (!m_clidSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ClassIDSvc.");
      return(StatusCode::FAILURE);
   }
   // Get scoped BackNavigation hints
   for (std::vector<std::string>::const_iterator iter = m_backNavigationScope.value().begin(),
		   last = m_backNavigationScope.value().end(); iter != last; iter++) {
      const std::string::size_type p_colon = iter->find("::");
      if (p_colon == std::string::npos) {
         ATH_MSG_FATAL("Unexpected format in BackNavigationScope: " << *iter);
         return(StatusCode::FAILURE);
      }
      std::pair<std::string, std::string> entry(iter->substr(p_colon + 2), iter->substr(0, p_colon));
      m_backNavigationMap.insert(entry);
      ATH_MSG_INFO("BackNavigationScope for: " << entry.first << " :: " << entry.second);
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode AthenaPoolAddressProviderSvc::finalize() {
   // Release ClassIDSvc
   if (!m_clidSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ClassIDSvc.");
   }
   return(::AthService::finalize());
}
//________________________________________________________________________________
StatusCode AthenaPoolAddressProviderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   return(::AthService::queryInterface(riid, ppvInterface));
}
//________________________________________________________________________________
StatusCode AthenaPoolAddressProviderSvc::preLoadAddresses(StoreID::type storeID,
		IAddressProvider::tadList& tads) {
   if (storeID != StoreID::DETECTOR_STORE) {
      return(StatusCode::SUCCESS);
   }
   ServiceHandle<StoreGateSvc> detectorStoreSvc("DetectorStore", name());
   if (!detectorStoreSvc.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Cannot get DetectorStore.");
      return(StatusCode::FAILURE);
   }
   std::list<SG::ObjectWithVersion<DataHeader> > allVersions;
   if (!detectorStoreSvc->retrieveAllVersions(allVersions, "CondProxyProvider").isSuccess()) {
      ATH_MSG_DEBUG("Cannot retrieve DataHeader from DetectorStore.");
      return(StatusCode::SUCCESS);
   }
   for (std::list<SG::ObjectWithVersion<DataHeader> >::const_iterator iter = allVersions.begin();
		   iter != allVersions.end(); iter++) {
      const DataHeader* dataHeader = iter->dataObject;
      ATH_MSG_DEBUG("The current File contains: " << dataHeader->size() << " objects");
      for (std::vector<DataHeaderElement>::const_iterator iter = dataHeader->begin(),
		      last = dataHeader->end(); iter != last; iter++) {
         SG::TransientAddress* tadd = iter->getAddress();
         if (tadd->clID() == ClassID_traits<DataHeader>::ID()) {
            delete tadd; tadd = 0;
         } else {
            ATH_MSG_DEBUG("preLoadAddresses: DataObject address, clid = " << tadd->clID() << ", name = " << tadd->name());
            tads.push_back(tadd);
         }
         EventSelectorAthenaPoolUtil::registerKeys(*iter, &*detectorStoreSvc);
      }
   }
   if (!detectorStoreSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release DetectorStoreSvc.");
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode AthenaPoolAddressProviderSvc::loadAddresses(StoreID::type storeID,
		IAddressProvider::tadList& tads) {
   if (storeID != StoreID::EVENT_STORE && storeID != StoreID::PILEUP_STORE) {
      return(StatusCode::SUCCESS);
   }

   Guid thisFile = Guid::null();
   long int oid2 = 0L;
   if (m_dataHeaderIterator) {
      const SG::DataProxy* dhProxy = eventStore()->proxy(ClassID_traits<DataHeader>::ID(), m_dataHeaderKey.value());
      if (dhProxy != 0 && dhProxy->address() != 0) {
         Token token;
         token.fromString(*dhProxy->address()->par());
         thisFile = token.dbID();
         oid2 = token.oid().second;
      }
   }
   if (thisFile == Guid::null() || oid2 == 0L || thisFile != m_guid) {
      const DataHandle<DataHeader> dataHeader;
      if (!eventStore()->retrieve(dataHeader, m_dataHeaderKey.value()).isSuccess() || !dataHeader.isValid()) {
         ATH_MSG_ERROR("Cannot retrieve DataHeader from StoreGate.");
         return(StatusCode::FAILURE);
      }
      dataHeader->setStatus(DataHeader::Primary);
      ATH_MSG_DEBUG("The current Event contains: " << dataHeader->size() << " objects");
      for (std::vector<DataHeaderElement>::const_iterator iter = dataHeader->begin(),
		      last = dataHeader->end(); iter != last; iter++) {
         SG::TransientAddress* tadd = iter->getAddress();
         if (tadd->clID() == ClassID_traits<DataHeader>::ID()) { // self reference
            if (tadd->name() != "StreamRAW") {
               if (tadd->name().empty() && dataHeader->sizeProvenance() == 1) { // reading DataHeader satellite
                  SG::TransientAddress* taddDh = dataHeader->beginProvenance()->getAddress();
                  if (taddDh != 0) {
                     tads.push_back(new SG::TransientAddress(taddDh->clID(), "Full", taddDh->address())); // full DataHeader
                  }
                  delete taddDh; taddDh = 0;
               }
               dataHeader->setProcessTag(tadd->name());
            }
            delete tadd; tadd = 0;
         } else {
            ATH_MSG_DEBUG("loadAddresses: DataObject address, clid = " << tadd->clID() << ", name = " << tadd->name());
            tadd->setProvider(this, storeID);
            if (m_dataHeaderIterator) {
               tadd->clearAddress(false);
            }
            tads.push_back(tadd);
         }
         EventSelectorAthenaPoolUtil::registerKeys(*iter, eventStore());
      }
      m_guid = thisFile;
   } else {
      std::vector<const SG::DataProxy*> preExistingProxies = eventStore()->proxies();
      for (std::vector<const SG::DataProxy*>::const_iterator iter = preExistingProxies.begin(),
		      last = preExistingProxies.end(); iter != last; iter++) {
         TokenAddress* tokAddr = dynamic_cast<TokenAddress*>((*iter)->address());
         if (tokAddr != 0 && tokAddr->getToken() != 0) {
            const_cast<Token*>(tokAddr->getToken())->oid().second = oid2;
         }
      }
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode AthenaPoolAddressProviderSvc::updateAddress(StoreID::type storeID,
                                                       SG::TransientAddress* tad,
                                                       const EventContext& /*ctx*/)
{
   assert(tad);
   if (storeID != StoreID::EVENT_STORE && storeID != StoreID::PILEUP_STORE) {
      return(StatusCode::FAILURE);
   }
   // No BackNavigation to DataHeader or AttributeList
   if (tad->clID() == ClassID_traits<DataHeader>::ID()) {
      return(StatusCode::FAILURE);
   } else if (tad->clID() == ClassID_traits<AthenaAttributeList>::ID()) {
      return(StatusCode::FAILURE);
   }
   std::string entry;
   const DataHandle<DataHeader> dataHeader;
   if (eventStore()->contains<DataHeader>("Full")) {
      if (!eventStore()->retrieve(dataHeader, "Full").isSuccess()) {
         ATH_MSG_ERROR("updateAddress: Cannot retrieve Full DataHeader from StoreGate");
         return(StatusCode::FAILURE);
      }
      const SG::TransientAddress* defObj = 0;
      for (std::vector<DataHeaderElement>::const_iterator iter = dataHeader->begin(),
		      last = dataHeader->end(); iter != last; iter++) {
         const SG::TransientAddress* tadd = iter->getAddress();
         std::set<CLID> ids = iter->getClassIDs();
         if (ids.find(tad->clID()) != ids.end()) {
            if (defObj == 0 && tad->name() == tadd->name()) {
               defObj = tadd; tadd = 0;
            } else if (tad->name() == "DEFAULT") {
               if (defObj == 0) {
                  defObj = tadd; tadd = 0;
               } else {
                  ATH_MSG_WARNING("No default Address for Clid: " << tad->clID());
                  delete tadd; tadd = 0;
                  delete defObj; defObj = 0;
                  return(StatusCode::FAILURE);
               }
            }
         }
         EventSelectorAthenaPoolUtil::registerKeys(*iter, eventStore());
         delete tadd; tadd = 0;
      }
      if (defObj !=0) {
         ATH_MSG_DEBUG("Found Address: " << defObj->address());
         tad->setAddress(defObj->address());
         delete defObj; defObj = 0;
         return(StatusCode::SUCCESS);
      }
   }
   if (!m_clidSvc->getTypeNameOfID(tad->clID(), entry).isSuccess()) {
      ATH_MSG_ERROR("updateAddress: Cannot translate clID for: " << tad->clID());
      return(StatusCode::FAILURE);
   }
   if (m_backNavigationMap.find(entry + "#" + tad->name()) != m_backNavigationMap.end()) {
      entry = m_backNavigationMap[entry + "#" + tad->name()];
   } else if (m_backNavigationMap.find(entry + "#*") != m_backNavigationMap.end()) {
      entry = m_backNavigationMap[entry + "#*"];
   } else if (m_backNavigationMap.find("*") != m_backNavigationMap.end()) {
      entry = m_backNavigationMap["*"];
   } else {
      entry.clear();
   }
   if (!m_backNavigationFlag.value() && entry.empty()) {
      return(StatusCode::FAILURE);
   }
   if (!eventStore()->contains<DataHeader>(m_dataHeaderKey.value())) {
      ATH_MSG_DEBUG("updateAddress: Cannot find DataHeader in StoreGate");
      return(StatusCode::FAILURE);
   }
   if (!eventStore()->retrieve(dataHeader, m_dataHeaderKey.value()).isSuccess()) {
      ATH_MSG_ERROR("updateAddress: Cannot retrieve DataHeader from StoreGate");
      return(StatusCode::FAILURE);
   }
   return(chaseAddress(tad, dataHeader.cptr(), entry));
}
//________________________________________________________________________________
StatusCode AthenaPoolAddressProviderSvc::chaseAddress(SG::TransientAddress* tad,
		const DataHeader* dataHeader,
		const std::string& processTag) {
   for (std::vector<DataHeaderElement>::const_iterator thisInputToken = dataHeader->beginProvenance(),
		   endInputToken = dataHeader->endProvenance(); thisInputToken != endInputToken;
		   thisInputToken++) {
      if (!m_backNavigationFlag.value() && processTag != thisInputToken->getKey() && !dataHeader->getProcessTag().empty()) {
         continue;
      }
      const Token* token = thisInputToken->getToken();
      const std::string key = token != 0 ? token->dbID().toString() : "";
      const DataHandle<DataHeader> pDataHeader;
      if (!eventStore()->contains<DataHeader>(key)) {
         GenericAddress* genAddr = new GenericAddress(thisInputToken->getStorageType(),
		         ClassID_traits<DataHeader>::ID(),
		         token != 0 ? token->toString() : "");
         if (!eventStore()->recordAddress(key, genAddr).isSuccess()) {
            ATH_MSG_ERROR("chaseAddress1: Cannot record Input DataHeader, key = " << key);
            return(StatusCode::FAILURE);
         }
      }
      if (!eventStore()->retrieve(pDataHeader, key).isSuccess() || pDataHeader.cptr() == 0) {
         ATH_MSG_ERROR("chaseAddress1: Cannot retrieve Input DataHeader, key = " << key);
         return(StatusCode::FAILURE);
      }
      const SG::TransientAddress* defObj = 0;
      for (std::vector<DataHeaderElement>::const_iterator iter = pDataHeader->begin(),
		      last = pDataHeader->end(); iter != last; iter++) {
         const SG::TransientAddress* tadd = iter->getAddress();
         std::set<CLID> ids = iter->getClassIDs();
         if (defObj == 0 && ids.find(tad->clID()) != ids.end() && tad->name() == tadd->name()) {
            defObj = tadd; tadd = 0;
         }
         EventSelectorAthenaPoolUtil::registerKeys(*iter, eventStore());
         delete tadd; tadd = 0;
      }
      if (defObj !=0) {
         ATH_MSG_DEBUG("Found Address: " << defObj->address());
         tad->setAddress(defObj->address());
         delete defObj; defObj = 0;
         return(StatusCode::SUCCESS);
      }
   }
   if (!m_backNavigationFlag.value() && processTag.empty()) {
      return(StatusCode::FAILURE);
   }
   std::vector<DataHeaderElement>::const_iterator thisInputToken = dataHeader->beginProvenance();
   if (thisInputToken != dataHeader->endProvenance()) {
      const Token* token = thisInputToken->getToken();
      const std::string key = token != 0 ? token->dbID().toString() : "";
      const DataHandle<DataHeader> pDataHeader;
      if (!eventStore()->contains<DataHeader>(key)) {
         GenericAddress* genAddr = new GenericAddress(thisInputToken->getStorageType(),
		         ClassID_traits<DataHeader>::ID(),
		         token != 0 ? token->toString() : "");
         if (!eventStore()->recordAddress(key, genAddr).isSuccess()) {
            ATH_MSG_ERROR("chaseAddress2: Cannot record Input DataHeader, key = " << key);
            return(StatusCode::FAILURE);
         }
         EventSelectorAthenaPoolUtil::registerKeys(*thisInputToken, eventStore());
      }
      if (!eventStore()->retrieve(pDataHeader, key).isSuccess()) {
         ATH_MSG_ERROR("chaseAddress2: Cannot retrieve Input DataHeader, key = " << key);
         return(StatusCode::FAILURE);
      }
      if (chaseAddress(tad, pDataHeader, processTag).isSuccess()) {
         return(StatusCode::SUCCESS);
      }
   }
   return(StatusCode::FAILURE);
}
