/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolConverter.cxx
 *  @brief This file contains the implementation for the AthenaPoolConverter base class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc/AthenaPoolConverter.h"
#include "AthenaPoolCnvSvc/IAthenaPoolCnvSvc.h"

#include "SGTools/DataProxy.h"

#include "PersistentDataModel/Guid.h"
namespace pool {
   typedef ::Guid Guid;
}
#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"
#include "StorageSvc/DbType.h"

//__________________________________________________________________________
AthenaPoolConverter::~AthenaPoolConverter() {
   delete m_placement; m_placement = nullptr;
   delete m_i_poolToken; m_i_poolToken = nullptr;
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::initialize() {
   if (!::Converter::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize Converter base class.");
      return(StatusCode::FAILURE);
   }
   // Retrieve AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get AthenaPoolCnvSvc.");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::finalize() {
   // Release AthenaPoolCnvSvc
   if (!m_athenaPoolCnvSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release AthenaPoolCnvSvc.");
   }
   return(::Converter::finalize());
}
//__________________________________________________________________________
long AthenaPoolConverter::repSvcType() const {
   return(POOL_StorageType);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
   std::lock_guard<CallMutex> lock(m_conv_mut);
   TokenAddress* tokAddr = dynamic_cast<TokenAddress*>(pAddr);
   bool ownTokAddr = false;
   if (tokAddr == nullptr || tokAddr->getToken() == nullptr) {
      ownTokAddr = true;
      Token* token = new Token;
      token->fromString(*(pAddr->par()));
      GenericAddress* genAddr = dynamic_cast<GenericAddress*>(pAddr);
      tokAddr = new TokenAddress(*genAddr, token);
   }
   m_i_poolToken = tokAddr->getToken();
   try {
      if (!PoolToDataObject(pObj, tokAddr->getToken()).isSuccess()) {
         ATH_MSG_ERROR("createObj PoolToDataObject() failed, Token = " << (tokAddr->getToken() ? tokAddr->getToken()->toString() : "NULL"));
         pObj = nullptr;
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createObj - caught exception: " << e.what());
      pObj = nullptr;
   }
   if (pObj == nullptr) {
      ATH_MSG_ERROR("createObj failed to get DataObject, Token = " << (tokAddr->getToken() ? tokAddr->getToken()->toString() : "NULL"));
   }
   if (ownTokAddr) {
      delete tokAddr; tokAddr = nullptr;
   }
   m_i_poolToken = nullptr;
   if (pObj == nullptr) {
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
   std::lock_guard<CallMutex> lock(m_conv_mut);
   const SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObj->registry());
   if (proxy == nullptr) {
      ATH_MSG_ERROR("AthenaPoolConverter CreateRep failed to cast DataProxy, key = "
             << pObj->registry()->name());
      return(StatusCode::FAILURE);
   }
   try {
      if (!DataObjectToPers(pObj, pObj->registry()->name()).isSuccess()) {
         ATH_MSG_ERROR("CreateRep failed, key = " << pObj->registry()->name());
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createRep - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   const CLID clid = proxy->clID();
   // Create a IOpaqueAddress for this object.
   pAddr = new TokenAddress(POOL_StorageType, clid, "", "", 0, 0);
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj) {
   std::lock_guard<CallMutex> lock(m_conv_mut);
   try {
      pObj->registry()->setAddress(pAddr);
      if (!DataObjectToPool(pObj, pObj->registry()->name()).isSuccess()) {
         ATH_MSG_ERROR("FillRepRefs failed, key = " << pObj->registry()->name());
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("fillRepRefs - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
long AthenaPoolConverter::storageType() {
   return(POOL_StorageType);
}
//__________________________________________________________________________
AthenaPoolConverter::AthenaPoolConverter(const CLID& myCLID, ISvcLocator* pSvcLocator,
                                         const char* name /*= nullptr*/) :
		::Converter(POOL_StorageType, myCLID, pSvcLocator),
		::AthMessaging((pSvcLocator != nullptr ? msgSvc() : nullptr),
                               name ? name : "AthenaPoolConverter"),
	m_athenaPoolCnvSvc("AthenaPoolCnvSvc", "AthenaPoolConverter"),
	m_placement(nullptr),
	m_placementHints(),
	m_className(),
	m_classDescs(),
	m_dataObject(nullptr),
	m_i_poolToken(nullptr) {
}
//__________________________________________________________________________
void AthenaPoolConverter::setPlacementWithType(const std::string& tname, const std::string& key) {
   if (m_placement == nullptr) {
      // Create placement for this converter if needed
      m_placement = new Placement();
   }
   // Set DB and Container names
   m_placement->setFileName(m_athenaPoolCnvSvc->getOutputConnectionSpec());
   std::string containerName;
   if (key.empty()) { // No key will result in a separate tree by type for the data object
      containerName = m_athenaPoolCnvSvc->getOutputContainer(tname);
   } else if (m_placementHints.find(tname + key) != m_placementHints.end()) { // PlacementHint already generated?
      containerName = m_placementHints[tname + key];
   } else { // Generate PlacementHint
      containerName = m_athenaPoolCnvSvc->getOutputContainer(tname, key);
      m_placementHints.insert(std::pair<std::string, std::string>(key, containerName));
   }
   m_placement->setTechnology(m_athenaPoolCnvSvc->technologyType(containerName).type());
   //  Remove Technology from containerName
   std::size_t colonPos = containerName.find(":");
   if (colonPos != std::string::npos) {
      containerName.erase(0, colonPos + 1);
   }
   m_placement->setContainerName(containerName);
}
//__________________________________________________________________________
const DataObject* AthenaPoolConverter::getDataObject() const {
   return(m_dataObject);
}
//__________________________________________________________________________
bool AthenaPoolConverter::compareClassGuid(const Guid &guid) const {
   return(m_i_poolToken ? (guid == m_i_poolToken->classID()) : false);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::cleanUp() {
   ATH_MSG_DEBUG("AthenaPoolConverter cleanUp called for base class.");
   return(StatusCode::SUCCESS);
}
