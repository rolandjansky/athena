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
   delete m_placement; m_placement = 0;
   delete m_i_poolToken; m_i_poolToken = 0;
   delete m_o_poolToken; m_o_poolToken = 0;
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
      if (m_i_poolToken == 0) m_i_poolToken = new Token;
      const_cast<Token*>(m_i_poolToken)->fromString(*(pAddr->par()));
   } else {
      m_i_poolToken = tokAddr->getToken();
   }
   try {
      if (!PoolToDataObject(pObj, m_i_poolToken).isSuccess()) {
         ATH_MSG_ERROR("createObj PoolToDataObject() failed, Token = " << (m_i_poolToken ? m_i_poolToken->toString() : "NULL"));
         pObj = 0;
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("createObj - caught exception: " << e.what());
      pObj = 0;
   }
   if (pObj == 0) {
      ATH_MSG_ERROR("createObj failed to get DataObject, Token = " << (m_i_poolToken ? m_i_poolToken->toString() : "NULL"));
   }
   if (tokAddr == 0 || tokAddr->getToken() == 0) {
      delete m_i_poolToken; m_i_poolToken = 0;
   } else {
      m_i_poolToken = 0;
   }
   if (pObj == 0) {
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolConverter::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
   // Create a Pool object for DataObject
   m_o_poolToken = 0;
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
   if ((m_o_poolToken == 0 || m_o_poolToken->classID() == Guid::null())) {
      ATH_MSG_ERROR("CreateRep failed to get Token, key = " << pObj->registry()->name());
      return(StatusCode::FAILURE);
   }
   const SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObj->registry());
   if (proxy == 0) {
      ATH_MSG_ERROR("AthenaPoolConverter CreateRep failed to cast DataProxy, key = "
	      << pObj->registry()->name());
      return(StatusCode::FAILURE);
   }
   const CLID clid = proxy->clID();
   // Create a IOpaqueAddress for this object.
   pAddr = new TokenAddress(POOL_StorageType, clid, "", "", 0, m_o_poolToken);
   m_o_poolToken = 0; // Token will be inserted into DataHeader, which takes ownership
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
	m_placementHints(),
	m_className(),
	m_classDescs(),
	m_dataObject(0),
	m_i_poolToken(0),
	m_o_poolToken(0) {
}
//__________________________________________________________________________
void AthenaPoolConverter::setPlacementWithType(const std::string& tname, const std::string& key) {
   if (m_placement == 0) {
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
