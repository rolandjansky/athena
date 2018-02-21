/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AuxDiscoverySvc.cxx
 *  @brief This file contains the implementation for the AuxDiscoverySvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AuxDiscoverySvc.h"

#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/normalizedTypeinfoName.h"
#include "AthContainersRoot/getDynamicAuxID.h"

#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "AthenaKernel/IAthenaIPCTool.h"

#include "PersistentDataModel/Guid.h"

#include "StorageSvc/DbTypeInfo.h"

#include "RootUtils/Type.h"
#include "TClass.h"

static SG::auxid_set_t s_emptySet;

class AthenaPoolAuxStore : public SG::AuxStoreInternal {
public:
  AthenaPoolAuxStore(bool standalone) : SG::AuxStoreInternal(standalone) {}
  using SG::AuxStoreInternal::addVector;
};

bool AuxDiscoverySvc::getAuxStore(void* obj, const Guid& classId, const std::string& contId) {
   pool::DbTypeInfo* info = pool::DbTypeInfo::create(classId); // Needed for Properties and TClass
   if (info == nullptr) {
      return false;
   }
   if ((contId.size() < 5 || contId.substr(contId.size() - 5, 4) != "Aux.")
	   && !info->clazz().Properties().HasProperty("IAuxStore")) {
      return false;
   }
   TClass* cl = info->clazz().Class();
   if (cl == nullptr) {
      return false;
   }
   TClass* holderTC = cl->GetBaseClass("SG::IAuxStoreHolder");
   if (holderTC == nullptr) {
      return false;
   }
   m_storeHolder = reinterpret_cast<SG::IAuxStoreHolder*>((char*)obj + cl->GetBaseClassOffset(holderTC));
   if (m_storeHolder == nullptr) {
      return false;
   }
   bool standalone = m_storeHolder->getStoreType() == SG::IAuxStoreHolder::AST_ObjectStore;
   m_storeInt = new AthenaPoolAuxStore(standalone);
   return true;
}

bool AuxDiscoverySvc::setData(SG::auxid_t auxid, void* data, const RootType& type) {
   SG::AuxTypeRegistry& registry = SG::AuxTypeRegistry::instance();
   if (m_storeInt->standalone()) {
      void* dstdata = m_storeInt->getData(auxid, 1, 1);
      registry.copy(auxid, dstdata, 0, data, 0);
      if (type.IsFundamental()) {
         delete [] (char*)data; data = nullptr;
      } else {
         type.Destruct(data); data = nullptr;
      }
   } else {
      // Move the data to the dynamic store.
      std::unique_ptr<SG::IAuxTypeVector> vec(registry.makeVectorFromData(auxid, data, false, true));
      m_storeInt->addVector(auxid, std::move(vec), false); data = nullptr;
   }
   return true;
}

bool AuxDiscoverySvc::setAuxStore() {
   if (m_storeHolder == nullptr) {
      return false;
   }
   m_storeHolder->setStore(m_storeInt);
   return true;
}

SG::auxid_t AuxDiscoverySvc::getAuxID(const std::string& attrName, const std::string& elemName, const std::string& typeName) {
   SG::AuxTypeRegistry& registry = SG::AuxTypeRegistry::instance();
   SG::auxid_t auxid = registry.findAuxID(attrName);
   if (auxid == SG::null_auxid) {
      RootUtils::Type elemType(elemName);
      const std::type_info* eti = elemType.getTypeInfo();
      if (eti == nullptr || m_storeInt == nullptr) {
         return SG::null_auxid;
      }
      auxid = SG::getDynamicAuxID(*eti, attrName, elemName, typeName, m_storeInt->standalone());
   }
   return auxid;
}

const SG::auxid_set_t& AuxDiscoverySvc::getAuxIDs(const void* obj, const Guid& classId, const std::string& contId) {
   pool::DbTypeInfo* info = pool::DbTypeInfo::create(classId); // Needed for Properties and TClass
   if (info == nullptr) {
      return s_emptySet;
   }
   if ((contId.size() < 5 || contId.substr(contId.size() - 5, 4) != "Aux.")
	   && !info->clazz().Properties().HasProperty("IAuxStore")) {
      return s_emptySet;
   }
   // Detected auxStore
   TClass* cl = info->clazz().Class();
   if (cl == nullptr) {
      return s_emptySet;
   }
   TClass* storeTC = cl->GetBaseClass("SG::IAuxStoreIO");
   if (storeTC == nullptr) {
      return s_emptySet;
   }
   m_store = reinterpret_cast<const SG::IAuxStoreIO*>((const char*)obj + cl->GetBaseClassOffset(storeTC));
   if (m_store == nullptr) {
      return s_emptySet;
   }
   return m_store->getSelectedAuxIDs();
}

const void* AuxDiscoverySvc::getData(SG::auxid_t auxid) {
   if (m_store == nullptr) {
      return nullptr;
   }
   return m_store->getIOData(auxid);
}

const std::type_info* AuxDiscoverySvc::getType(SG::auxid_t auxid) {
   if (m_store == nullptr) {
      return nullptr;
   }
   return m_store->getIOType(auxid);
}

std::string AuxDiscoverySvc::getAttrName(SG::auxid_t auxid) {
   return SG::AuxTypeRegistry::instance().getName(auxid);
}

std::string AuxDiscoverySvc::getTypeName(SG::auxid_t auxid) {
   return SG::normalizedTypeinfoName(*(getType(auxid)));
}

std::string AuxDiscoverySvc::getElemName(SG::auxid_t auxid) {
   return SG::AuxTypeRegistry::instance().getTypeName(auxid);
}
StatusCode AuxDiscoverySvc::receiveStore(const IAthenaSerializeSvc* serSvc, const IAthenaIPCTool* ipcTool, void* obj, int num) {
   void* buffer = nullptr;
   size_t nbytes = 0;
   StatusCode sc = ipcTool->getObject(&buffer, nbytes, num);
   while (sc.isRecoverable() && nbytes > 0) {
      sc = ipcTool->getObject(&buffer, nbytes, num);
   }
   if (!sc.isSuccess() || nbytes == 0) { // No dynamic attributes
      return(StatusCode::SUCCESS);
   }
   Guid classId;
   classId.fromString(static_cast<char*>(buffer));
   if (!ipcTool->getObject(&buffer, nbytes, num).isSuccess() || nbytes == 0) {
      return(StatusCode::FAILURE);
   }
   const std::string contName = std::string(static_cast<char*>(buffer));
   if (classId != Guid::null() && !contName.empty() && this->getAuxStore(obj, classId, contName)) {
      void* attrName = nullptr;
      void* typeName = nullptr;
      void* elemName = nullptr;
      // StreamingTool owns buffer, will stay around until last dynamic attribute is copied
      while (ipcTool->getObject(&attrName, nbytes, num).isSuccess() && nbytes > 0 &&
	      ipcTool->getObject(&typeName, nbytes, num).isSuccess() && nbytes > 0 &&
	      ipcTool->getObject(&elemName, nbytes, num).isSuccess() && nbytes > 0) {
         if (ipcTool->getObject(&buffer, nbytes, num).isSuccess()) {
            const RootType type(std::string(static_cast<char*>(typeName)));
            void* dynAttr = nullptr;
            if (type.IsFundamental()) {
               dynAttr = new char[nbytes];
               std::memcpy(dynAttr, buffer, nbytes); buffer = nullptr;
            } else {
               dynAttr = serSvc->deserialize(buffer, nbytes, type); buffer = nullptr;
            }
            SG::auxid_t auxid = this->getAuxID(static_cast<char*>(attrName),
	            static_cast<char*>(elemName),
	            static_cast<char*>(typeName));
            this->setData(auxid, dynAttr, type);
         }
      }
      this->setAuxStore();
   }
   return(StatusCode::SUCCESS);
}

StatusCode AuxDiscoverySvc::sendStore(const IAthenaSerializeSvc* serSvc,
		const IAthenaIPCTool* ipcTool,
		const void* obj,
		const Guid& classId,
		const std::string& contName,
		int num) {
   const SG::auxid_set_t& auxIDs = this->getAuxIDs(obj, classId, contName);
   if (!auxIDs.empty()) {
      const std::string& classIdStr = classId.toString();
      if (!ipcTool->putObject(classIdStr.c_str(), classIdStr.size() + 1, num).isSuccess()) {
         return(StatusCode::FAILURE);
      }
      if (!ipcTool->putObject(contName.c_str(), contName.size() + 1, num).isSuccess()) {
         return(StatusCode::FAILURE);
      }
   }
   for (SG::auxid_set_t::const_iterator iter = auxIDs.begin(), last = auxIDs.end(); iter != last; iter++) {
      const std::string& attrName = this->getAttrName(*iter);
      const std::string& typeName = this->getTypeName(*iter);
      const std::string& elemName = this->getElemName(*iter);
      if (!ipcTool->putObject(attrName.c_str(), attrName.size() + 1, num).isSuccess() ||
	      !ipcTool->putObject(typeName.c_str(), typeName.size() + 1, num).isSuccess() ||
	      !ipcTool->putObject(elemName.c_str(), elemName.size() + 1, num).isSuccess()) {
         return(StatusCode::FAILURE);
      }
      const std::type_info* tip = this->getType(*iter);
      if (tip == nullptr) {
         return(StatusCode::FAILURE);
      }
      RootType type(*tip);
      StatusCode sc = StatusCode::FAILURE;
      if (type.IsFundamental()) {
         sc = ipcTool->putObject(this->getData(*iter), type.SizeOf(), num);
      } else {
         size_t nbytes = 0;
         void* buffer = serSvc->serialize(this->getData(*iter), type, nbytes);
         sc = ipcTool->putObject(buffer, nbytes, num);
         delete [] static_cast<char*>(buffer); buffer = nullptr;
      }
      if (!sc.isSuccess()) {
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
