/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AUXDISCOVERYSVC_H
#define AUXDISCOVERYSVC_H

/** @file AuxDiscoverySvc.h
 *  @brief This file contains the class definition for the AuxDiscoverySvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/StatusCode.h"

#include "AthContainersInterfaces/AuxTypes.h"
#include "DataModelRoot/RootType.h"

#include <string>

// Forward declarations
class Guid;
class IAthenaSerializeSvc;
class IAthenaIPCTool;
class AthenaPoolAuxStore;
namespace SG {
   class IAuxStoreIO;
   class IAuxStoreHolder;
}

/** @class AuxDiscoverySvc 
 *  @brief This class provides the interface between AthenaPoolCnvSvc and AuxStore classes.
 **/
class AuxDiscoverySvc {
public:
   AuxDiscoverySvc() : m_store(0), m_storeInt(0), m_storeHolder(0) {}

   bool getAuxStore(void* obj, const Guid& classId, const std::string& contId);

   bool setData(SG::auxid_t auxid, void* data, const RootType& type);

   bool setAuxStore();

   SG::auxid_t getAuxID(const std::string& attrName, const std::string& elemName, const std::string& typeName);

   const SG::auxid_set_t& getAuxIDs(const void* obj, const Guid& classId, const std::string& contId);

   const void* getData(SG::auxid_t auxid);

   const std::type_info* getType(SG::auxid_t auxid);

   std::string getAttrName(SG::auxid_t auxid);

   std::string getTypeName(SG::auxid_t auxid);

   std::string getElemName(SG::auxid_t auxid);

   /// Receive dynamic aux store variables from streaming tool
   StatusCode receiveStore(const IAthenaSerializeSvc* serSvc, const IAthenaIPCTool* ipcTool, void* obj, int num = 0);

   /// Send dynamic aux store variables to streaming tool
   StatusCode sendStore(const IAthenaSerializeSvc* serSvc,
	   const IAthenaIPCTool* ipcTool,
	   const void* obj,
	   const Guid& classId,
	   const std::string& contName,
	   int num = 0);

private: // data
   const SG::IAuxStoreIO* m_store;
   AthenaPoolAuxStore* m_storeInt;
   SG::IAuxStoreHolder* m_storeHolder;
};

#endif
