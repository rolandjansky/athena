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
   delete m_o_poolToken; m_o_poolToken = nullptr;
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
   TokenAddress* tokAddr = dynamic_cast<TokenAddress*>(pAddr);
   if (tokAddr == nullptr || tokAddr->getToken() == nullptr) {
      if (m_i_poolToken == nullptr) m_i_poolToken = new Token;
      const_cast<Token*>(m_i_poolToken)->fromString(*(pAddr->par()));
   } else {
      m_i_poolToken = tokAddr->getToken();
   }
   if (m_i_poolToken != nullptr) {
      char text[32];
      if (*(pAddr->ipar()) != IPoolSvc::kInputStream) {
// Use ipar field of GenericAddress to create custom input context/persSvc in PoolSvc::setObjPtr() (e.g. for conditions)
         ::sprintf(text, "[CTXT=%08X]", static_cast<int>(*(pAddr->ipar())));
// Or use context label, e.g.: ::sprintf(text, "[CLABEL=%08X]", pAddr->clID()); to create persSvc
         const_cast<Token*>(m_i_poolToken)->setAuxString(text);
      }
   }
   try {
      if (!PoolToDataObject(pObj, m_i_poolToken).isSuccess()) {
         ATH_MSG_ERROR("createObj PoolToDataObject() failed, Token = " << (m_i_poolToken ? m_i_poolToken->toString() : "NULL"));
         pObj = nullptr;
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createObj - caught exception: " << e.what());
      pObj = nullptr;
   }
   if (pObj == nullptr) {
      ATH_MSG_ERROR("createObj failed to get DataObject, Token = " << (m_i_poolToken ? m_i_poolToken->toString() : "NULL"));
   }
   if (tokAddr == nullptr || tokAddr->getToken() == nullptr) {
      delete m_i_poolToken; m_i_poolToken = nullptr;
   } else {
      m_i_poolToken = nullptr;
   }
   if (pObj == nullptr) {
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
   // Create a Pool object for DataObject
   m_o_poolToken = nullptr;
   try {
      if (!DataObjectToPool(pObj, pObj->registry()->name()).isSuccess()) {
         ATH_MSG_ERROR("CreateRep failed, key = " << pObj->registry()->name());
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createRep - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   // Null/empty token means ERROR
   if (m_o_poolToken == nullptr || m_o_poolToken->classID() == Guid::null()) {
      ATH_MSG_ERROR("CreateRep failed to get Token, key = " << pObj->registry()->name());
      return(StatusCode::FAILURE);
   }
   const SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObj->registry());
   if (proxy == nullptr) {
      ATH_MSG_ERROR("AthenaPoolConverter CreateRep failed to cast DataProxy, key = "
	      << pObj->registry()->name());
      return(StatusCode::FAILURE);
   }
   const CLID clid = proxy->clID();
   // Create a IOpaqueAddress for this object.
   pAddr = new TokenAddress(POOL_StorageType, clid, "", "", 0, m_o_poolToken);
   m_o_poolToken = nullptr; // Token will be inserted into DataHeader, which takes ownership
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
long AthenaPoolConverter::storageType() {
   return(POOL_StorageType);
}
//__________________________________________________________________________
AthenaPoolConverter::AthenaPoolConverter(const CLID& myCLID, ISvcLocator* pSvcLocator) :
		::Converter(POOL_StorageType, myCLID, pSvcLocator),
		::AthMessaging((pSvcLocator != nullptr ? msgSvc() : nullptr), "AthenaPoolConverter"),
	m_athenaPoolCnvSvc("AthenaPoolCnvSvc", "AthenaPoolConverter"),
	m_placement(nullptr),
	m_placementHints(),
	m_className(),
	m_classDescs(),
	m_dataObject(nullptr),
	m_i_poolToken(nullptr),
	m_o_poolToken(nullptr) {
}
//__________________________________________________________________________
void AthenaPoolConverter::setPlacementWithType(const std::string& tname, const std::string& key) {
   if (m_placement == nullptr) {
      // Create placement for this converter if needed
      m_placement = new Placement();
   }
   // Use technology from AthenaPoolCnvSvc
   if (m_athenaPoolCnvSvc->technologyType().type() == 0) {
      // error if type is 0
      ATH_MSG_WARNING("technology UNDEFINED for type " << tname);
   }
   m_placement->setTechnology(m_athenaPoolCnvSvc->technologyType().type());
   // Set DB and Container names
   const std::string fileName = m_athenaPoolCnvSvc->getOutputConnectionSpec();
   m_placement->setFileName(fileName);
   if (key.empty()) { // No key will result in a separate tree by type for the data object
      m_placement->setContainerName(m_athenaPoolCnvSvc->getOutputContainer(tname));
   } else if (m_placementHints.find(tname + key) != m_placementHints.end()) { // PlacementHint already generated?
      m_placement->setContainerName(m_placementHints[tname + key]);
   } else { // Generate PlacementHint
      const std::string containerName = m_athenaPoolCnvSvc->getOutputContainer(tname, key);
      const std::pair<std::string, std::string> entry(key, containerName);
      m_placementHints.insert(entry);
      m_placement->setContainerName(containerName);
   }
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
