/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "PoolSvc/IPoolSvc.h"

// Framework
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"

// Pool
#include "CollectionBase/ICollectionCursor.h"

#include <vector>

//________________________________________________________________________________
CondProxyProvider::CondProxyProvider(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthService(name, pSvcLocator),
	m_athenaPoolCnvSvc("AthenaPoolCnvSvc", name),
	m_poolCollectionConverter(0),
	m_headerIterator(0) {
   declareProperty("InputCollections", m_inputCollectionsProp);
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
   m_headerIterator = 0;
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
   // Create DataHeader iterators
   m_headerIterator = &m_poolCollectionConverter->executeQuery();
   if (!m_headerIterator->next()) {
      if (m_poolCollectionConverter != 0) {
         m_poolCollectionConverter->disconnectDb().ignore();
         delete m_poolCollectionConverter; m_poolCollectionConverter = 0;
      }
      m_inputCollectionsIterator++;
      if (m_inputCollectionsIterator != m_inputCollectionsProp.value().end()) {
         // Create PoolCollectionConverter for input file
         m_poolCollectionConverter = getCollectionCnv();
         if (m_poolCollectionConverter == 0) {
            return(StatusCode::FAILURE);
         }
         // Get DataHeader iterator
         m_headerIterator = &m_poolCollectionConverter->executeQuery();
         if (!m_headerIterator->next()) {
            return(StatusCode::FAILURE);
         }
      }
   }
   GenericAddress* genAddr = new GenericAddress(POOL_StorageType,
	   ClassID_traits<DataHeader>::ID(),
	   m_headerIterator->eventRef().toString(),
	   name());
   if (!detectorStoreSvc->recordAddress(genAddr).isSuccess()) {
      ATH_MSG_ERROR("Cannot record DataHeader.");
      return(StatusCode::FAILURE);
   }
   if (!detectorStoreSvc->contains<DataHeader>("CondProxyProvider")) {
      ATH_MSG_DEBUG("Cannot find DataHeader in DetectorStore.");
      return(StatusCode::SUCCESS);
   }
   const DataHandle<DataHeader> dataHeader;
   if (!detectorStoreSvc->retrieve(dataHeader, "CondProxyProvider").isSuccess()) {
      ATH_MSG_DEBUG("Cannot retrieve DataHeader from DetectorStore.");
      return(StatusCode::SUCCESS);
   }
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
