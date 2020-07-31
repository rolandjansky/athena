/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file CondProxyProvider.cxx
 *  @brief This file contains the implementation for the CondProxyProvider class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "CondProxyProvider.h"
#include "PoolCollectionConverter.h"
#include "registerKeys.h"

#include "AthenaPoolCnvSvc/IAthenaPoolCnvSvc.h"
#include "PersistentDataModel/DataHeader.h"
#include "PersistentDataModel/TokenAddress.h"
#include "PoolSvc/IPoolSvc.h"

// Framework
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

// Pool
#include "CollectionBase/ICollectionCursor.h"

#include <vector>

//________________________________________________________________________________
CondProxyProvider::CondProxyProvider(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthService(name, pSvcLocator),
	m_athenaPoolCnvSvc("AthenaPoolCnvSvc", name),
	m_poolCollectionConverter(0)
	{
}
//________________________________________________________________________________
CondProxyProvider::~CondProxyProvider() {
}
//________________________________________________________________________________
StatusCode CondProxyProvider::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }
   // Check for input collection
   if (m_inputCollectionsProp.value().size() == 0) {
      return(StatusCode::FAILURE);
   }
   // Retrieve AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get AthenaPoolCnvSvc.");
      return(StatusCode::FAILURE);
   }
   // Get PersistencySvc
   StatusCode status = m_athenaPoolCnvSvc->getPoolSvc()->connect(pool::ITransaction::READ, IPoolSvc::kInputStream);
   if (!status.isSuccess()) {
      ATH_MSG_FATAL("Cannot connect to Database.");
      return(StatusCode::FAILURE);
   }
   // Initialize
   m_inputCollectionsIterator = m_inputCollectionsProp.value().begin();
   // Create an m_poolCollectionConverter to read the objects in
   m_poolCollectionConverter = getCollectionCnv();
   if (m_poolCollectionConverter == 0) {
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode CondProxyProvider::finalize() {
   if (m_poolCollectionConverter != 0) {
      m_poolCollectionConverter->disconnectDb().ignore();
      delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
   }
   // Release AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release AthenaPoolCnvSvc.");
   }
   return(::AthService::finalize());
}
//________________________________________________________________________________
StatusCode CondProxyProvider::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   return(::AthService::queryInterface(riid, ppvInterface));
}
//________________________________________________________________________________
StatusCode CondProxyProvider::preLoadAddresses(StoreID::type storeID,
		IAddressProvider::tadList& tads) {
   if (storeID != StoreID::DETECTOR_STORE) {
      return(StatusCode::SUCCESS);
   }
   ServiceHandle<StoreGateSvc> detectorStoreSvc("DetectorStore", name());
   // Retrieve DetectorStoreSvc
   if (!detectorStoreSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get DetectorStoreSvc.");
      return(StatusCode::FAILURE);
   }

   if (m_poolCollectionConverter == nullptr) {
     return StatusCode::FAILURE;
   }
   
   // Create DataHeader iterators
   pool::ICollectionCursor* headerIterator = &m_poolCollectionConverter->executeQuery();
   for (int verNumber = 0; verNumber < 100; verNumber++) {
      if (!headerIterator->next()) {
         m_poolCollectionConverter->disconnectDb().ignore();
         delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
         m_inputCollectionsIterator++;
         if (m_inputCollectionsIterator != m_inputCollectionsProp.value().end()) {
            // Create PoolCollectionConverter for input file
            m_poolCollectionConverter = getCollectionCnv();
            if (m_poolCollectionConverter == 0) {
               return(StatusCode::FAILURE);
            }
            // Get DataHeader iterator
            headerIterator = &m_poolCollectionConverter->executeQuery();
            if (!headerIterator->next()) {
               return(StatusCode::FAILURE);
            }
         } else {
            break;
         }
      }
      SG::VersionedKey myVersKey(name(), verNumber);
      Token* token = new Token;
      token->fromString(headerIterator->eventRef().toString());
      TokenAddress* tokenAddr = new TokenAddress(POOL_StorageType, ClassID_traits<DataHeader>::ID(), "", myVersKey, IPoolSvc::kInputStream, token);
      if (!detectorStoreSvc->recordAddress(tokenAddr).isSuccess()) {
         ATH_MSG_ERROR("Cannot record DataHeader.");
         return(StatusCode::FAILURE);
      }
   }
   std::list<SG::ObjectWithVersion<DataHeader> > allVersions;
   if (!detectorStoreSvc->retrieveAllVersions(allVersions, name()).isSuccess()) {
      ATH_MSG_DEBUG("Cannot retrieve DataHeader from DetectorStore.");
      return(StatusCode::SUCCESS);
   }
   for (std::list<SG::ObjectWithVersion<DataHeader> >::iterator iter = allVersions.begin();
                   iter != allVersions.end(); iter++) {
      SG::ReadHandle<DataHeader> dataHeader = iter->dataObject;
      ATH_MSG_DEBUG("The current File contains: " << dataHeader->size() << " objects");
      for (const auto& element : *dataHeader) {
         SG::TransientAddress* tadd = element.getAddress();
         if (tadd->clID() == ClassID_traits<DataHeader>::ID()) {
            delete tadd; tadd = 0;
         } else {
            ATH_MSG_DEBUG("preLoadAddresses: DataObject address, clid = " << tadd->clID() << ", name = " << tadd->name());
            tads.push_back(tadd);
         }
         EventSelectorAthenaPoolUtil::registerKeys(element, &*detectorStoreSvc);
      }
   }
   if (!detectorStoreSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release DetectorStoreSvc.");
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode CondProxyProvider::loadAddresses(StoreID::type /*storeID*/,
	IAddressProvider::tadList& /*tads*/) {
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode CondProxyProvider::updateAddress(StoreID::type /*storeID*/,
                                            SG::TransientAddress* /*tad*/,
                                            const EventContext& /*ctx*/) {
   return(StatusCode::FAILURE);
}
//__________________________________________________________________________
PoolCollectionConverter* CondProxyProvider::getCollectionCnv() {
   ATH_MSG_DEBUG("Try item: \"" << *m_inputCollectionsIterator << "\" from the collection list.");
   PoolCollectionConverter* pCollCnv = new PoolCollectionConverter("ImplicitROOT",
	   *m_inputCollectionsIterator,
	   "",
	   m_athenaPoolCnvSvc->getPoolSvc());
   if (!pCollCnv->initialize().isSuccess()) {
      // Close previous collection.
      delete pCollCnv; pCollCnv = 0;
      ATH_MSG_ERROR("Unable to open: " << *m_inputCollectionsIterator);
   }
   return(pCollCnv);
}
