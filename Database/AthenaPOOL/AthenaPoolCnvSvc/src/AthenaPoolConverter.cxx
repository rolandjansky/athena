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

#include "PersistencySvc/Placement.h"
#include "PersistentDataModel/Guid.h"
namespace pool {
   typedef ::Guid Guid;
}
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"
#include "StorageSvc/DbType.h"

//__________________________________________________________________________
AthenaPoolConverter::~AthenaPoolConverter() {
   delete m_placement; m_placement = 0;
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
   if (tokAddr == 0 || tokAddr->getToken() == 0) {
      if (m_poolToken == 0) m_poolToken = new Token;
      const_cast<Token*>(m_poolToken)->fromString(*(pAddr->par()));
   } else {
      m_poolToken = tokAddr->getToken();
      m_token.clear();
   }
   m_dataObject = 0;
   try {
      if (!PoolToDataObject(pObj, "").isSuccess()) {
         ATH_MSG_ERROR("createObj PoolToDataObject() failed, Token = " << (m_poolToken ? m_poolToken->toString() : m_token));
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createObj - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   m_dataObject = pObj;
   if (m_dataObject == 0) {
      ATH_MSG_ERROR("createObj failed to get DataObject, Token = " << (m_poolToken ? m_poolToken->toString() : m_token));
      return(StatusCode::FAILURE);
   }
   if (tokAddr == 0 || tokAddr->getToken() == 0) {
      delete m_poolToken; m_poolToken = 0;
   } else {
      m_poolToken = 0;
   }
   m_token.clear();
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
   // Create a Pool object for DataObject
   m_dataObject = pObj;
   m_poolToken = 0;
   m_token.clear();
   try {
      if (!DataObjectToPool(pObj, m_dataObject->registry()->name()).isSuccess()) {
         ATH_MSG_ERROR("CreateRep failed, key = " << m_dataObject->registry()->name());
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createRep - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   // Null/empty token means ERROR
   if ((m_poolToken == 0 || m_poolToken->classID() == Guid::null()) && m_token.empty()) {
      ATH_MSG_ERROR("CreateRep failed to get Token, key = " << m_dataObject->registry()->name());
      return(StatusCode::FAILURE);
   }
   const SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObj->registry());
   if (proxy == 0) {
      ATH_MSG_ERROR("AthenaPoolConverter CreateRep failed to cast DataProxy, key = "
	      << m_dataObject->registry()->name());
      return(StatusCode::FAILURE);
   }
   const CLID clid = proxy->clID();
   // Create a IOpaqueAddress for this object.
   pAddr = new TokenAddress(POOL_StorageType, clid, "", "", 0, m_poolToken);
   m_poolToken = 0; // Token will be inserted into DataHeader, which takes ownership
   m_token.clear();
   m_dataObject = 0;
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
long AthenaPoolConverter::storageType() {
   return(POOL_StorageType);
}
//__________________________________________________________________________
AthenaPoolConverter::AthenaPoolConverter(const CLID& myCLID, ISvcLocator* pSvcLocator) :
		::Converter(POOL_StorageType, myCLID, pSvcLocator),
		::AthMessaging((pSvcLocator != 0 ? msgSvc() : 0), "AthenaPoolConverter"),
	m_athenaPoolCnvSvc("AthenaPoolCnvSvc", "AthenaPoolConverter"),
	m_placement(0),
	m_dictionaryOkRead(false),
	m_dictionaryOkWrite(false),
	m_placementHints(),
	m_className(),
	m_classDescs(),
	m_token(),
	m_dataObject(0),
	m_poolToken(0) {
}
//__________________________________________________________________________
void AthenaPoolConverter::setPlacementWithType(const std::string& tname, const std::string& key) {
   if (m_placement == 0) {
      // Create placement for this converter if needed
      m_placement = new pool::Placement();
   }
   // Use technology from AthenaPoolCnvSvc
   if (m_athenaPoolCnvSvc->technologyType().type() == 0) {
      // error if type is 0
      ATH_MSG_WARNING("technology UNDEFINED for type " << tname);
   }
   m_placement->setTechnology(m_athenaPoolCnvSvc->technologyType().type());
   // Set DB and Container names
   const std::string fileName = m_athenaPoolCnvSvc->getOutputConnectionSpec();
   m_placement->setDatabase(fileName, pool::DatabaseSpecification::PFN);
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
DataObject* AthenaPoolConverter::getDataObject() const {
   return(m_dataObject);
}
//__________________________________________________________________________
void AthenaPoolConverter::setDataObject(DataObject* pObj) {
   m_dataObject = pObj;
}
//__________________________________________________________________________
bool AthenaPoolConverter::compareClassGuid(const Guid &guid) const {
   return(m_poolToken ? (guid == m_poolToken->classID()) : false);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::cleanUp() {
   ATH_MSG_DEBUG("AthenaPoolConverter cleanUp called for base class.");
   return(StatusCode::SUCCESS);
}
