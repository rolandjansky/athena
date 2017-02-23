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

#include "PersistentDataModel/Token.h"

#include "StorageSvc/DbTypeInfo.h"

#include "RootUtils/Type.h" //FIXME: Avoid new dependency
#include "TClass.h"

static SG::auxid_set_t s_emptySet;

class AthenaPoolAuxStore : public SG::AuxStoreInternal {
public:
  AthenaPoolAuxStore(bool standalone) : SG::AuxStoreInternal(standalone) {}
  using SG::AuxStoreInternal::addVector;
};

bool AuxDiscoverySvc::getAuxStore(void* obj, const Token& token) {
   pool::DbTypeInfo* info = pool::DbTypeInfo::create(token.classID()); // Needed for Properties and TClass
   if (info == nullptr) {
      return false;
   }
   if ((token.contID().size() < 5 || token.contID().substr(token.contID().size() - 5, 4) != "Aux.")
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

const SG::auxid_set_t& AuxDiscoverySvc::getAuxIDs(void* obj, const Token& token) {
   pool::DbTypeInfo* info = pool::DbTypeInfo::create(token.classID()); // Needed for Properties and TClass
   if (info == nullptr) {
      return s_emptySet;
   }
   if ((token.contID().size() < 5 || token.contID().substr(token.contID().size() - 5, 4) != "Aux.")
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
   m_store = reinterpret_cast<SG::IAuxStoreIO*>((char*)obj + cl->GetBaseClassOffset(storeTC));
   if (m_store == nullptr) {
      return s_emptySet;
   }
   return m_store->getDynamicAuxIDs();
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
