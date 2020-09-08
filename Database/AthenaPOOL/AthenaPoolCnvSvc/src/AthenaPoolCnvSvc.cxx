/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolCnvSvc.cxx
 *  @brief This file contains the implementation for the AthenaPoolCnvSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/FileIncident.h"

#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/AttribStringParser.h"

#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "AthenaKernel/IMetaDataSvc.h"
#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"
#include "PersistentDataModel/DataHeader.h"


#include "StorageSvc/DbReflex.h"

#include "AuxDiscoverySvc.h"

#include "boost/algorithm/string.hpp"
#include <algorithm>

//______________________________________________________________________________
// Initialize the service.
StatusCode AthenaPoolCnvSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthCnvSvc::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize ConversionSvc base class.");
      return(StatusCode::FAILURE);
   }

   // Initialize DataModelCompatSvc
   ServiceHandle<IService> dmcsvc("DataModelCompatSvc", this->name());
   if (!dmcsvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get DataModelCompatSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve PoolSvc
   if (!m_poolSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get PoolSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve ChronoStatSvc
   if (!m_chronoStatSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ChronoStatSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve ClassIDSvc
   if (!m_clidSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ClassIDSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve InputStreamingTool (if configured)
   if (!m_inputStreamingTool.empty()) {
      if (!m_inputStreamingTool.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get Input AthenaIPCTool");
         return(StatusCode::FAILURE);
      }
   }
   // Retrieve OutputStreamingTool (if configured)
   if (!m_outputStreamingTool.empty()) {
      m_streamClientFiles = m_streamClientFilesProp.value();
      if (!m_outputStreamingTool.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get Output AthenaIPCTool");
         return(StatusCode::FAILURE);
      }
   }
   if (!m_inputStreamingTool.empty() || !m_outputStreamingTool.empty()) {
      // Retrieve AthenaSerializeSvc
      if (!m_serializeSvc.retrieve().isSuccess()) {
         ATH_MSG_FATAL("Cannot get AthenaSerializeSvc.");
         return(StatusCode::FAILURE);
      }
   }
   // Extracting MaxFileSizes for global default and map by Database name.
   for (std::vector<std::string>::const_iterator iter = m_maxFileSizes.value().begin(),
	   last = m_maxFileSizes.value().end(); iter != last; ++iter) {
      if (iter->find('=') != std::string::npos) {
         long long maxFileSize = atoll(iter->substr(iter->find('=') + 1).c_str());
         if (maxFileSize > 15000000000LL) {
            ATH_MSG_INFO("Files larger than 15GB are disallowed by ATLAS policy.");
            ATH_MSG_INFO("They should only be produced for private use or in special cases.");
         }
         std::string databaseName = iter->substr(0, iter->find_first_of(" 	="));
         std::pair<std::string, long long> entry(databaseName, maxFileSize);
         m_databaseMaxFileSize.insert(entry);
      } else {
         m_domainMaxFileSize = atoll(iter->c_str());
         if (m_domainMaxFileSize > 15000000000LL) {
            ATH_MSG_INFO("Files larger than 15GB are disallowed by ATLAS policy.");
            ATH_MSG_INFO("They should only be produced for private use or in special cases.");
         }
      }
   }
   // Extracting INPUT POOL ItechnologySpecificAttributes for Domain, Database and Container.
   extractPoolAttributes(m_inputPoolAttr, &m_inputAttr, &m_inputAttr, &m_inputAttr);
   // Extracting the INPUT POOL ItechnologySpecificAttributes which are to be printed for each event
   extractPoolAttributes(m_inputPoolAttrPerEvent, &m_inputAttrPerEvent, &m_inputAttrPerEvent, &m_inputAttrPerEvent);
   if (!m_inputPoolAttrPerEvent.value().empty()) {
      // Setup incident for EndEvent to print out attributes each event
      ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
      long int pri = 1000;
      // Set to be listener for EndEvent
      incSvc->addListener(this, "EndEvent", pri);
      ATH_MSG_DEBUG("Subscribed to EndEvent for printing out input file attributes.");
   }
   if (!processPoolAttributes(m_inputAttr, "", IPoolSvc::kInputStream, false, true, true).isSuccess()) {
      ATH_MSG_DEBUG("setInputAttribute failed setting POOL domain attributes.");
   }
   m_doChronoStat = m_skipFirstChronoCommit.value() ? false : true;
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::finalize() {
   // Release AthenaSerializeSvc
   if (!m_serializeSvc.empty()) {
      if (!m_serializeSvc.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release AthenaSerializeSvc.");
      }
   }
   // Release OutputStreamingTool (if configured)
   if (!m_outputStreamingTool.empty()) {
      if (!m_outputStreamingTool.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release Output AthenaIPCTool.");
      }
   }
   // Release InputStreamingTool (if configured)
   if (!m_inputStreamingTool.empty()) {
      if (!m_inputStreamingTool.release().isSuccess()) {
         ATH_MSG_WARNING("Cannot release Input AthenaIPCTool.");
      }
   }
   // Release ClassIDSvc
   if (!m_clidSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ClassIDSvc.");
   }
   // Release ChronoStatSvc
   if (!m_chronoStatSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ChronoStatSvc.");
   }
   // Release PoolSvc
   if (!m_poolSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release PoolSvc.");
   }

   m_cnvs.clear();
   m_cnvs.shrink_to_fit();
   return(::AthCnvSvc::finalize());
}
//_______________________________________________________________________
StatusCode AthenaPoolCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (IAthenaPoolCnvSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<IAthenaPoolCnvSvc*>(this);
   } else {
      // Interface is not directly available: try out a base class
      return(::AthCnvSvc::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::createObj(IOpaqueAddress* pAddress, DataObject*& refpObject) {
   assert(pAddress);
   std::string objName = "ALL";
   if (m_useDetailChronoStat.value()) {
      if (m_clidSvc->getTypeNameOfID(pAddress->clID(), objName).isFailure()) {
         std::ostringstream oss;
         oss << std::dec << pAddress->clID();
         objName = oss.str();
      }
      objName += "#" + *(pAddress->par() + 1);
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStart("cObj_" + objName);
   }
   TokenAddress* tokAddr = dynamic_cast<TokenAddress*>(pAddress);
   if (tokAddr != nullptr && tokAddr->getToken() != nullptr) {
      char text[32];
      // Use ipar field of GenericAddress to create custom input context/persSvc in PoolSvc::setObjPtr() (e.g. for conditions)
      ::sprintf(text, "[CTXT=%08X]", static_cast<int>(*(pAddress->ipar())));
      // Or use context label, e.g.: ::sprintf(text, "[CLABEL=%08X]", pAddress->clID()); to create persSvc
      tokAddr->getToken()->setAuxString(text);
   }
   // Forward to base class createObj
   StatusCode status = ::AthCnvSvc::createObj(pAddress, refpObject);
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStop("cObj_" + objName);
   }
   return(status);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::createRep(DataObject* pObject, IOpaqueAddress*& refpAddress) {
   assert(pObject);
   std::string objName = "ALL";
   if (m_useDetailChronoStat.value()) {
      if (m_clidSvc->getTypeNameOfID(pObject->clID(), objName).isFailure()) {
         std::ostringstream oss;
         oss << std::dec << pObject->clID();
         objName = oss.str();
      }
      objName += "#" + pObject->registry()->name();
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStart("cRep_" + objName);
   }
   StatusCode status = StatusCode::FAILURE;
   if (pObject->clID() == 1) {
      // No transient object was found use cnv to write default persistent object
      SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObject->registry());
      if (proxy != nullptr) {
         IConverter* cnv = converter(proxy->clID());
         status = cnv->createRep(pObject, refpAddress);
      }
   } else {
      // Forward to base class createRep
      try {
         status = ::AthCnvSvc::createRep(pObject, refpAddress);
      } catch(std::runtime_error& e) {
         ATH_MSG_FATAL(e.what());
      }
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStop("cRep_" + objName);
   }
   return(status);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::fillRepRefs(IOpaqueAddress* pAddress, DataObject* pObject) {
   assert(pObject);
   std::string objName = "ALL";
   if (m_useDetailChronoStat.value()) {
      if (m_clidSvc->getTypeNameOfID(pObject->clID(), objName).isFailure()) {
         std::ostringstream oss;
         oss << std::dec << pObject->clID();
         objName = oss.str();
      }
      objName += "#" + pObject->registry()->name();
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStart("fRep_" + objName);
   }
   StatusCode status = StatusCode::FAILURE;
   if (pObject->clID() == 1) {
      // No transient object was found use cnv to write default persistent object
      SG::DataProxy* proxy = dynamic_cast<SG::DataProxy*>(pObject->registry());
      if (proxy != nullptr) {
         IConverter* cnv = converter(proxy->clID());
         status = cnv->fillRepRefs(pAddress, pObject);
      }
   } else {
      // Forward to base class fillRepRefs
      try {
         status = ::AthCnvSvc::fillRepRefs(pAddress, pObject);
      } catch(std::runtime_error& e) {
         ATH_MSG_FATAL(e.what());
      }
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStop("fRep_" + objName);
   }
   return(status);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::connectOutput(const std::string& outputConnectionSpec,
		const std::string& /*openMode*/) {
   return(connectOutput(outputConnectionSpec));
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::connectOutput(const std::string& outputConnectionSpec) {
// This is called before DataObjects are being converted.
   std::string outputConnection = outputConnectionSpec.substr(0, outputConnectionSpec.find("["));
   // Extract the technology
   int tech = m_dbType.type();
   if (!decodeOutputSpec(outputConnection, tech).isSuccess()) {
      ATH_MSG_ERROR("connectOutput FAILED extract file name and technology.");
      return(StatusCode::FAILURE);
   }
   if (m_makeStreamingToolClient.value() > 0 && !m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isServer() && !m_outputStreamingTool[0]->isClient()) {
      m_outputStreamingTool[0]->makeClient(m_makeStreamingToolClient.value()).ignore();
   }
   if (!m_outputStreamingTool.empty() && m_outputStreamingTool[0]->isClient()
	   && (!m_streamMetaDataOnly || outputConnectionSpec.find("[PoolContainerPrefix=" + m_metadataContainerProp.value() + "]") != std::string::npos)) {
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient()) {
      if (m_streamMetaDataOnly && outputConnectionSpec.find("[PoolContainerPrefix=" + m_metadataContainerProp.value() + "]") == std::string::npos) {
         ATH_MSG_DEBUG("connectOutput SKIPPED for metadata-only server: " << outputConnectionSpec);
         return(StatusCode::SUCCESS);
      }
      if (!m_streamMetaDataOnly && (m_streamServer == m_outputStreamingTool.size() || !m_outputStreamingTool[m_streamServer < m_outputStreamingTool.size() ? m_streamServer : 0]->isServer())) {
         ATH_MSG_DEBUG("connectOutput SKIPPED for expired server.");
         return(StatusCode::SUCCESS);
      }
      std::size_t streamClient = 0;
      for (std::vector<std::string>::const_iterator iter = m_streamClientFiles.begin(), last = m_streamClientFiles.end(); iter != last; iter++) {
         if (*iter == outputConnection) break;
         streamClient++;
      }
      if (streamClient == m_streamClientFiles.size()) {
         m_streamClientFiles.push_back(outputConnection);
      }
   }

   unsigned int contextId = outputContextId(outputConnection);
   try {
      if (!m_poolSvc->connect(pool::ITransaction::UPDATE, contextId).isSuccess()) {
         ATH_MSG_ERROR("connectOutput FAILED to open an UPDATE transaction.");
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("connectOutput - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }

   std::unique_lock<std::mutex> lock(m_mutex);
   if (std::find(m_contextAttr.begin(), m_contextAttr.end(), contextId) == m_contextAttr.end()) {
      m_contextAttr.push_back(contextId);
      // Setting default 'TREE_MAX_SIZE' for ROOT to 1024 GB to avoid file chains.
      std::vector<std::string> maxFileSize;
      maxFileSize.push_back("TREE_MAX_SIZE");
      maxFileSize.push_back("1099511627776L");
      m_domainAttr.push_back(maxFileSize);
      // Extracting OUTPUT POOL ItechnologySpecificAttributes for Domain, Database and Container.
      extractPoolAttributes(m_poolAttr, &m_containerAttr, &m_databaseAttr, &m_domainAttr);
   }
   if (!processPoolAttributes(m_domainAttr, outputConnection, contextId).isSuccess()) {
      ATH_MSG_DEBUG("connectOutput failed process POOL domain attributes.");
   }
   if (!processPoolAttributes(m_databaseAttr, outputConnection, contextId).isSuccess()) {
      ATH_MSG_DEBUG("connectOutput failed process POOL database attributes.");
   }
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::commitOutput(const std::string& outputConnectionSpec, bool doCommit) {
// This is called after all DataObjects are converted.
   std::string outputConnection = outputConnectionSpec.substr(0, outputConnectionSpec.find("["));
   if (!m_outputStreamingTool.empty() && m_outputStreamingTool[0]->isClient()
	   && (!m_streamMetaDataOnly || outputConnectionSpec.find("[PoolContainerPrefix=" + m_metadataContainerProp.value() + "]") != std::string::npos)) {
      std::size_t streamClient = 0;
      for (std::vector<std::string>::const_iterator iter = m_streamClientFiles.begin(), last = m_streamClientFiles.end(); iter != last; iter++) {
         if (*iter == outputConnection) break;
         streamClient++;
      }
      if (streamClient == m_streamClientFiles.size()) {
         if (m_streamClientFiles.size() < m_outputStreamingTool.size()) {
            m_streamClientFiles.push_back(outputConnection);
         } else {
            streamClient = 0;
         }
      }
      StatusCode sc = m_outputStreamingTool[streamClient]->lockObject("release");
      while (sc.isRecoverable()) {
         usleep(100);
         sc = m_outputStreamingTool[streamClient]->lockObject("release");
      }
      if (!this->cleanUp(outputConnection).isSuccess()) {
         ATH_MSG_ERROR("commitOutput FAILED to cleanup converters.");
         return(StatusCode::FAILURE);
      }
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient() && m_metadataContainerProp.value().empty()
                  && m_streamServer == m_outputStreamingTool.size()) {
      ATH_MSG_DEBUG("commitOutput SKIPPED for expired server.");
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient() && !m_outputStreamingTool[m_streamServer < m_outputStreamingTool.size() ? m_streamServer : 0]->isServer()) {
      ATH_MSG_DEBUG("commitOutput SKIPPED for uninitialized server: " << m_streamServer << ".");
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient() && m_streamServer == m_outputStreamingTool.size()) {
      std::size_t streamClient = 0;
      for (std::vector<std::string>::const_iterator iter = m_streamClientFiles.begin(), last = m_streamClientFiles.end(); iter != last; iter++) {
         if (*iter == outputConnection) break;
         streamClient++;
      }
      if (streamClient == m_streamClientFiles.size()) {
         ATH_MSG_DEBUG("commitOutput SKIPPED for unconnected file: " << outputConnection << ".");
         return(StatusCode::SUCCESS);
      }
   }
   std::map<void*, RootType> commitCache;
   std::string fileName;
   if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient() && m_streamServer < m_outputStreamingTool.size()
		   && m_outputStreamingTool[m_streamServer]->isServer()) {
      auto& streamingTool = m_outputStreamingTool[m_streamServer];
      // Clear object to get Placements for all objects in a Stream
      char* placementStr = nullptr;
      int num = -1;
      StatusCode sc = streamingTool->clearObject(&placementStr, num);
      if (sc.isSuccess() && placementStr != nullptr && strlen(placementStr) > 6 && num > 0) {
         fileName = strstr(placementStr, "[FILE=");
         fileName = fileName.substr(6, fileName.find(']') - 6);
         if (!this->connectOutput(fileName).isSuccess()) {
            ATH_MSG_ERROR("Failed to connectOutput for " << fileName);
            return abortSharedWrClients(num);
         }
         IConverter* DHcnv = converter(ClassID_traits<DataHeader>::ID());
         bool dataHeaderSeen = false;
         std::string dataHeaderID;
         while (num > 0 && strncmp(placementStr, "release", 7) != 0) {
            std::string objName = "ALL";
            if (m_useDetailChronoStat.value()) {
               std::string objName(placementStr); //FIXME, better descriptor
            }
            if (m_doChronoStat) {
               m_chronoStatSvc->chronoStart("cRep_" + objName);
            }
            std::string tokenStr = placementStr;
            std::string contName = strstr(placementStr, "[CONT=");
            tokenStr = tokenStr.substr(0, tokenStr.find("[CONT=")) + contName.substr(contName.find(']') + 1);
            contName = contName.substr(6, contName.find(']') - 6);
            std::string className = strstr(placementStr, "[PNAME=");
            className = className.substr(7, className.find(']') - 7);
            RootType classDesc = RootType::ByNameNoQuiet(className);
            void* obj = nullptr;
            std::ostringstream oss2;
            oss2 << std::dec << num;
            std::string::size_type len = m_metadataContainerProp.value().size();
            if (len > 0 && contName.substr(0, len) == m_metadataContainerProp.value()
		            && contName.substr(len, 1) == "(") {
               ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
               // For Metadata, before moving to next client, fire file incidents
               if (m_metadataClient != num) {
                  if (m_metadataClient != 0) {
                     std::ostringstream oss1;
                     oss1 << std::dec << m_metadataClient;
                     std::string memName = "SHM[NUM=" + oss1.str() + "]";
                     FileIncident beginInputIncident(name(), "BeginInputFile", memName);
                     incSvc->fireIncident(beginInputIncident);
                     FileIncident endInputIncident(name(), "EndInputFile", memName);
                     incSvc->fireIncident(endInputIncident);
                  }
                  m_metadataClient = num;
               }
 	       // Retrieve MetaDataSvc
	       ServiceHandle<IMetaDataSvc> metadataSvc("MetaDataSvc", name());
	       ATH_CHECK(metadataSvc.retrieve());
	       if(!metadataSvc->shmProxy(std::string(placementStr) + "[NUM=" + oss2.str() + "]").isSuccess()) {
                  ATH_MSG_FATAL("MetaDataSvc::shmProxy() failed!");
                  return abortSharedWrClients(num);
	       }
            } else {
               Token readToken;
               readToken.setOid(Token::OID_t(num, 0));
               readToken.setAuxString("[PNAME=" + className + "]");
               this->setObjPtr(obj, &readToken); // Pull/read Object out of shared memory
               if (len == 0 || contName.substr(0, len) != m_metadataContainerProp.value()) {
                  // Write object
                  Placement placement;
                  placement.fromString(placementStr); placementStr = nullptr;
                  std::unique_ptr<Token> token(registerForWrite(&placement, obj, classDesc));
                  if (token == nullptr) {
                     ATH_MSG_ERROR("Failed to write Data for: " << className);
                     return abortSharedWrClients(num);
                  }
                  tokenStr = token->toString();
                  if (className == "DataHeader_p6") {
                     // Found DataHeader
                     GenericAddress address(POOL_StorageType, ClassID_traits<DataHeader>::ID(),
                                            tokenStr, placement.auxString());
                     // call DH converter to add the ref to DHForm (stored earlier) and to itself
                     if (!DHcnv->updateRep(&address, static_cast<DataObject*>(obj)).isSuccess()) {
                        ATH_MSG_ERROR("Failed updateRep for obj = " << tokenStr);
                        return abortSharedWrClients(num);
                     }
                     dataHeaderSeen = true;
                     dataHeaderID = token->contID() + "/" + oss2.str();
                  } else if (dataHeaderSeen) {
                     dataHeaderSeen = false;
                     // next object after DataHeader - may be a DataHeaderForm
                     // in any case we need to call the DH converter to update the DHForm Ref
                     if (className == "DataHeaderForm_p6") {
                        // Tell DataHeaderCnv that it should use a new DHForm
                        GenericAddress address(POOL_StorageType, ClassID_traits<DataHeader>::ID(),
                                               tokenStr, dataHeaderID);
                        if (!DHcnv->updateRepRefs(&address, static_cast<DataObject*>(obj)).isSuccess()) {
                           ATH_MSG_ERROR("Failed updateRepRefs for obj = " << tokenStr);
                           return abortSharedWrClients(num);
                        }
                     } else {
                        // Tell DataHeaderCnv that it should use the old DHForm
                        GenericAddress address(0, 0, "", dataHeaderID);
                        if (!DHcnv->updateRepRefs(&address, nullptr).isSuccess()) {
                           ATH_MSG_ERROR("Failed updateRepRefs for DataHeader");
                           return abortSharedWrClients(num);
                        }
                     }
                  }
                  if (className != "Token" && className != "DataHeaderForm_p6" && !classDesc.IsFundamental()) {
                     commitCache.insert(std::pair<void*, RootType>(obj, classDesc));
                  }
               }
            }
            // Send Token back to Client
            sc = streamingTool->lockObject(tokenStr.c_str(), num);
            while (sc.isRecoverable()) {
               sc = streamingTool->lockObject(tokenStr.c_str(), num);
            }
            if (!sc.isSuccess()) {
               ATH_MSG_ERROR("Failed to lock Data for " << tokenStr);
               return abortSharedWrClients(-1);
            }
            if (m_doChronoStat) {
               m_chronoStatSvc->chronoStop("cRep_" + objName);
            }
            sc = streamingTool->clearObject(&placementStr, num);
            while (sc.isRecoverable()) {
               sc = streamingTool->clearObject(&placementStr, num);
            }
            if (sc.isFailure()) {
               // no more clients, break the loop and exit
               num = -1;
            }
         }
         if (dataHeaderSeen) {
            // DataHeader was the last object, need to tell the converter there is no DHForm coming
            GenericAddress address(0, 0, "", dataHeaderID);
            if (!DHcnv->updateRepRefs(&address, nullptr).isSuccess()) {
               ATH_MSG_ERROR("Failed updateRepRefs for DataHeader");
               return abortSharedWrClients(-1);
            }
         }
         placementStr = nullptr;
      } else if (sc.isSuccess() && placementStr != nullptr && strncmp(placementStr, "stop", 4) == 0) {
         return(StatusCode::RECOVERABLE);
      } else if (sc.isRecoverable() || num == -1) {
         return(StatusCode::RECOVERABLE);
      }
      if (sc.isFailure() || fileName.empty()) {
         ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
         std::ostringstream oss1;
         oss1 << std::dec << m_metadataClient;
         std::string memName = "SHM[NUM=" + oss1.str() + "]";
         FileIncident beginInputIncident(name(), "BeginInputFile", memName);
         incSvc->fireIncident(beginInputIncident);
         FileIncident endInputIncident(name(), "EndInputFile", memName);
         incSvc->fireIncident(endInputIncident);
         if (sc.isFailure()) {
            ATH_MSG_INFO("All SharedWriter clients stopped - exiting");
         } else {
            ATH_MSG_INFO("Failed to get Data for client: " << num);
         }
         return(StatusCode::FAILURE);
      }
   }
   if (m_streamMetaDataOnly && !fileName.empty()) {
      ATH_MSG_DEBUG("commitOutput SKIPPED for metadata-only server: " << outputConnectionSpec);
      return(StatusCode::SUCCESS);
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStart("commitOutput");
   }
   std::unique_lock<std::mutex> lock(m_mutex);
   if (outputConnection.empty()) {
      outputConnection = fileName;
   }
   // Extract the technology
   int tech = m_dbType.type();
   if (!decodeOutputSpec(outputConnection, tech).isSuccess()) {
      ATH_MSG_ERROR("connectOutput FAILED extract file name and technology.");
      return(StatusCode::FAILURE);
   }
   unsigned int contextId = outputContextId(outputConnection);
   if (!processPoolAttributes(m_domainAttr, outputConnection, contextId).isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL domain attributes.");
   }
   if (!processPoolAttributes(m_databaseAttr, outputConnection, contextId).isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL database attributes.");
   }
   if (!processPoolAttributes(m_containerAttr, outputConnection, contextId).isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL container attributes.");
   }
   // lock.unlock();  //MN: first need to make commitCache slot-specific
   try {
      if (doCommit) {
         if (!m_poolSvc->commit(contextId).isSuccess()) {
            ATH_MSG_ERROR("commitOutput FAILED to commit OutputStream.");
            return(StatusCode::FAILURE);
         }
      } else {
         if (!m_poolSvc->commitAndHold(contextId).isSuccess()) {
            ATH_MSG_ERROR("commitOutput FAILED to commitAndHold OutputStream.");
            return(StatusCode::FAILURE);
         }
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("commitOutput - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   if (!this->cleanUp(outputConnection).isSuccess()) {
      ATH_MSG_ERROR("commitOutput FAILED to cleanup converters.");
      return(StatusCode::FAILURE);
   }
   for (std::map<void*, RootType>::iterator iter = commitCache.begin(), last = commitCache.end(); iter != last; ++iter) {
      iter->second.Destruct(iter->first);
   }
   // Check FileSize
   long long int currentFileSize = m_poolSvc->getFileSize(outputConnection, m_dbType.type(), contextId);
   if (m_databaseMaxFileSize.find(outputConnection) != m_databaseMaxFileSize.end()) {
      if (currentFileSize > m_databaseMaxFileSize[outputConnection]) {
         ATH_MSG_WARNING("FileSize > " << m_databaseMaxFileSize[outputConnection] << " for " << outputConnection);
         return(StatusCode::RECOVERABLE);
      }
   } else if (currentFileSize > m_domainMaxFileSize) {
      ATH_MSG_WARNING("FileSize > " << m_domainMaxFileSize <<  " for " << outputConnection);
      return(StatusCode::RECOVERABLE);
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStop("commitOutput");
   }
   // Prepare chrono for next commit
   m_doChronoStat = true;
   return(StatusCode::SUCCESS);
}

//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::disconnectOutput(const std::string& outputConnectionSpec) {
   std::string outputConnection = outputConnectionSpec.substr(0, outputConnectionSpec.find("["));
   if (!m_outputStreamingTool.empty() && m_outputStreamingTool[0]->isClient()
	   && (!m_streamMetaDataOnly || outputConnectionSpec.find("[PoolContainerPrefix=" + m_metadataContainerProp.value() + "]") != std::string::npos)) {
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient()) {
      if (m_metadataContainerProp.value().empty() && m_streamServer == m_outputStreamingTool.size()) {
         ATH_MSG_DEBUG("disconnectOutput SKIPPED for expired server.");
         return(StatusCode::SUCCESS);
      } else if (!m_metadataContainerProp.value().empty() && m_streamServer < m_outputStreamingTool.size()) {
         m_streamServer = m_outputStreamingTool.size();
         ATH_MSG_DEBUG("disconnectOutput SKIPPED to expire server.");
         return(StatusCode::SUCCESS);
      } else {
         m_streamServer = m_outputStreamingTool.size();
      }
      ATH_MSG_DEBUG("disconnectOutput not SKIPPED for server: " << m_streamServer);
   }
   return m_poolSvc->disconnect(outputContextId(outputConnection));
}

//______________________________________________________________________________
unsigned int AthenaPoolCnvSvc::outputContextId(const std::string& outputConnection) {
   return m_persSvcPerOutput?
      m_poolSvc->getOutputContext(outputConnection) : (unsigned int)IPoolSvc::kOutputStream;
}

//______________________________________________________________________________
IPoolSvc* AthenaPoolCnvSvc::getPoolSvc() {
   return(&*m_poolSvc);
}
//______________________________________________________________________________
Token* AthenaPoolCnvSvc::registerForWrite(Placement* placement, const void* obj, const RootType& classDesc) {
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStart("cRepR_ALL");
   }
   if (m_makeStreamingToolClient.value() > 0 && !m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isServer() && !m_outputStreamingTool[0]->isClient()) {
      m_outputStreamingTool[0]->makeClient(m_makeStreamingToolClient.value()).ignore();
   }
   Token* token = nullptr;
   if (!m_outputStreamingTool.empty() && m_outputStreamingTool[0]->isClient()
	   && (!m_streamMetaDataOnly || placement->containerName().substr(0, m_metadataContainerProp.value().size()) == m_metadataContainerProp.value())) {
      std::size_t streamClient = 0;
      std::string fileName = placement->fileName();
      for (std::vector<std::string>::const_iterator iter = m_streamClientFiles.begin(), last = m_streamClientFiles.end(); iter != last; iter++) {
         if (*iter == fileName) break;
         streamClient++;
      }
      if (streamClient == m_streamClientFiles.size()) {
         if (m_streamClientFiles.size() < m_outputStreamingTool.size()) {
            m_streamClientFiles.push_back(fileName);
         } else {
            streamClient = 0;
         }
      }
      // Lock object
      std::string placementStr = placement->toString() + "[PNAME=" + classDesc.Name() + "]";
      ATH_MSG_VERBOSE("Requesting write object for: " << placementStr);
      StatusCode sc = m_outputStreamingTool[streamClient]->lockObject(placementStr.c_str());
      while (sc.isRecoverable()) {
         //usleep(100);
         sc = m_outputStreamingTool[streamClient]->lockObject(placementStr.c_str());
      }
      if (!sc.isSuccess()) {
         ATH_MSG_ERROR("Failed to lock Data for " << placementStr);
         return(nullptr);
      }
      // Serialize object via ROOT
      const void* buffer = nullptr;
      std::size_t nbytes = 0;
      bool own = true;
      if (classDesc.Name() == "Token") {
         nbytes = strlen(static_cast<const char*>(obj)) + 1;
         buffer = obj;
         own = false;
      } else if (classDesc.IsFundamental()) {
         nbytes = classDesc.SizeOf();
         buffer = obj;
         own = false;
      } else {
         buffer = m_serializeSvc->serialize(obj, classDesc, nbytes);
      }
      // Share object
      sc = m_outputStreamingTool[streamClient]->putObject(buffer, nbytes);
      while (sc.isRecoverable()) {
         //usleep(100);
         sc = m_outputStreamingTool[streamClient]->putObject(buffer, nbytes);
      }
      if (own) { delete [] static_cast<const char*>(buffer); }
      buffer = nullptr;
      if (!sc.isSuccess()) {
         ATH_MSG_ERROR("Could not share object for: " << placementStr);
         m_outputStreamingTool[streamClient]->putObject(nullptr, 0).ignore();
         return(nullptr);
      }
      AuxDiscoverySvc auxDiscover;
      if (!auxDiscover.sendStore(m_serializeSvc.get(), m_outputStreamingTool[streamClient].get(), obj, pool::DbReflex::guid(classDesc), placement->containerName()).isSuccess()) {
         ATH_MSG_ERROR("Could not share dynamic aux store for: " << placementStr);
         m_outputStreamingTool[streamClient]->putObject(nullptr, 0).ignore();
         return(nullptr);
      }
      if (!m_outputStreamingTool[streamClient]->putObject(nullptr, 0).isSuccess()) {
         ATH_MSG_ERROR("Failed to put Data for " << placementStr);
         return(nullptr);
      }
      // Get Token back from Server
      char* tokenStr = nullptr;
      int num = -1;
      sc = m_outputStreamingTool[streamClient]->clearObject(&tokenStr, num);
      while (sc.isRecoverable()) {
         //usleep(100);
         sc = m_outputStreamingTool[streamClient]->clearObject(&tokenStr, num);
      }
      if (!sc.isSuccess()) {
         ATH_MSG_ERROR("Failed to get Token");
         return(nullptr);
      }
      if (!strcmp(tokenStr, "ABORT")) {
         ATH_MSG_ERROR("Writer requested ABORT");
         // tell the server we are leaving
         m_outputStreamingTool[streamClient]->stop().ignore();
         return nullptr;
      }
      Token* tempToken = new Token();
      tempToken->fromString(tokenStr); tokenStr = nullptr;
      tempToken->setClassID(pool::DbReflex::guid(classDesc));
      token = tempToken; tempToken = nullptr;
// Client Write Request
   } else {
      if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient() && m_metadataContainerProp.value().empty()
		      && (m_streamServer == m_outputStreamingTool.size() || !m_outputStreamingTool[m_streamServer < m_outputStreamingTool.size() ? m_streamServer : 0]->isServer())) {
         ATH_MSG_DEBUG("registerForWrite SKIPPED for expired server, Placement = " << placement->toString());
         Token* tempToken = new Token();
         tempToken->setClassID(pool::DbReflex::guid(classDesc));
         token = tempToken; tempToken = nullptr;
      } else if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient() && m_streamServer != m_outputStreamingTool.size() && !m_outputStreamingTool[m_streamServer < m_outputStreamingTool.size() ? m_streamServer : 0]->isServer()) {
         ATH_MSG_DEBUG("registerForWrite SKIPPED for uninitialized server, Placement = " << placement->toString());
         Token* tempToken = new Token();
         tempToken->setClassID(pool::DbReflex::guid(classDesc));
         token = tempToken; tempToken = nullptr;
      } else if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient() && m_streamServer == m_outputStreamingTool.size()) {
         std::size_t streamClient = 0;
         std::string fileName = placement->fileName();
         for (std::vector<std::string>::const_iterator iter = m_streamClientFiles.begin(), last = m_streamClientFiles.end(); iter != last; iter++) {
            if (*iter == fileName) break;
            streamClient++;
         }
         if (streamClient == m_streamClientFiles.size()) {
            ATH_MSG_DEBUG("registerForWrite SKIPPED for unconnected file: " << fileName << ".");
            Token* tempToken = new Token();
            tempToken->setClassID(pool::DbReflex::guid(classDesc));
            token = tempToken; tempToken = nullptr;
         } else {
            ATH_MSG_VERBOSE("Requested write object for: " << placement->toString());
            token = m_poolSvc->registerForWrite(placement, obj, classDesc);
         }
      } else {
         if (m_persSvcPerOutput) {
            char text[32];
            ::sprintf(text, "[CTXT=%08X]", m_poolSvc->getOutputContext(placement->fileName()));
            placement->setAuxString(text);
         }
         token = m_poolSvc->registerForWrite(placement, obj, classDesc);
      }
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStop("cRepR_ALL");
   }
   return(token);
}
//______________________________________________________________________________
void AthenaPoolCnvSvc::setObjPtr(void*& obj, const Token* token) const {
   ATH_MSG_VERBOSE("Requesting object for: " << token->toString());
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStart("cObjR_ALL");
   }
   if (!m_outputStreamingTool.empty() && m_streamServer < m_outputStreamingTool.size()
		   && m_outputStreamingTool[m_streamServer]->isServer()) {
      if (token->dbID() == Guid::null()) {
         int num = token->oid().first;
         // Get object from SHM
         void* buffer = nullptr;
         std::size_t nbytes = 0;
         StatusCode sc = m_outputStreamingTool[m_streamServer]->getObject(&buffer, nbytes, num);
         while (sc.isRecoverable()) {
            //usleep(100);
            sc = m_outputStreamingTool[m_streamServer]->getObject(&buffer, nbytes, num);
         }
         if (!sc.isSuccess()) {
            ATH_MSG_ERROR("Failed to get Data for " << token->toString());
            obj = nullptr;
         } else {
            if (token->classID() != Guid::null()) {
               // Deserialize object
               RootType cltype(pool::DbReflex::forGuid(token->classID()));
               obj = m_serializeSvc->deserialize(buffer, nbytes, cltype); buffer = nullptr;
            } else {
               // Deserialize object
               std::string className = token->auxString();
               className = className.substr(className.find("[PNAME="));
               className = className.substr(7, className.find(']') - 7);
               RootType cltype(RootType::ByNameNoQuiet(className));
               obj = m_serializeSvc->deserialize(buffer, nbytes, cltype); buffer = nullptr;
            }
            AuxDiscoverySvc auxDiscover;
            if (!auxDiscover.receiveStore(m_serializeSvc.get(), m_outputStreamingTool[m_streamServer].get(), obj, num).isSuccess()) {
               ATH_MSG_ERROR("Failed to get Dynamic Aux Store for " << token->toString());
               obj = nullptr;
            }
         }
      }
   }
   if (!m_inputStreamingTool.empty() && m_inputStreamingTool->isClient()) {
      ATH_MSG_VERBOSE("Requesting object for: " << token->toString());
      if (!m_inputStreamingTool->lockObject(token->toString().c_str()).isSuccess()) {
         ATH_MSG_ERROR("Failed to lock Data for " << token->toString());
         obj = nullptr;
      } else {
         if (m_doChronoStat) {
            m_chronoStatSvc->chronoStart("gObj_ALL");
         }
         void* buffer = nullptr;
         std::size_t nbytes = 0;
         StatusCode sc = m_inputStreamingTool->getObject(&buffer, nbytes);
         while (sc.isRecoverable()) {
            // sleep
            sc = m_inputStreamingTool->getObject(&buffer, nbytes);
         }
         if (m_doChronoStat) {
            m_chronoStatSvc->chronoStop("gObj_ALL");
         }
         if (!sc.isSuccess()) {
            ATH_MSG_ERROR("Failed to get Data for " << token->toString());
            obj = nullptr;
         } else {
            obj = m_serializeSvc->deserialize(buffer, nbytes, token->classID()); buffer = nullptr;
            AuxDiscoverySvc auxDiscover;
            if (!auxDiscover.receiveStore(m_serializeSvc.get(), m_inputStreamingTool.get(), obj).isSuccess()) {
               ATH_MSG_ERROR("Failed to get Dynamic Aux Store for " << token->toString());
               obj = nullptr;
            }
         }
      }
   } else if (token->dbID() != Guid::null()) {
      ATH_MSG_VERBOSE("Requesting object for: " << token->toString());
      m_poolSvc->setObjPtr(obj, token);
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStop("cObjR_ALL");
   }
}
//______________________________________________________________________________
bool AthenaPoolCnvSvc::useDetailChronoStat() const {
   return(m_useDetailChronoStat.value());
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::createAddress(long svcType,
		const CLID& clid,
		const std::string* par,
		const unsigned long* ip,
		IOpaqueAddress*& refpAddress) {
   if (svcType != POOL_StorageType) {
      ATH_MSG_ERROR("createAddress: svcType != POOL_StorageType " << svcType << " " << POOL_StorageType);
      return(StatusCode::FAILURE);
   }
   Token* token = nullptr;
   if (par[0].substr(0, 3) == "SHM") {
      token = new Token();
      token->setOid(Token::OID_t(ip[0], ip[1]));
      token->setAuxString("[PNAME=" + par[2] + "]");
      RootType classDesc = RootType::ByNameNoQuiet(par[2]);
      token->setClassID(pool::DbReflex::guid(classDesc));
   } else if (!m_inputStreamingTool.empty() && m_inputStreamingTool->isClient()) {
      Token addressToken;
      addressToken.setDb(par[0].substr(4));
      addressToken.setCont(par[1]);
      addressToken.setOid(Token::OID_t(ip[0], ip[1]));
      if (!m_inputStreamingTool->lockObject(addressToken.toString().c_str()).isSuccess()) {
         ATH_MSG_WARNING("Failed to lock Address Token: " << addressToken.toString());
         return(StatusCode::FAILURE);
      }
      void* buffer = nullptr;
      std::size_t nbytes = 0;
      StatusCode sc = m_inputStreamingTool->getObject(&buffer, nbytes);
      while (sc.isRecoverable()) {
         // sleep
         sc = m_inputStreamingTool->getObject(&buffer, nbytes);
      }
      if (!sc.isSuccess()) {
         ATH_MSG_WARNING("Failed to get Address Token: " << addressToken.toString());
         return(StatusCode::FAILURE);
      }
      token = new Token();
      token->fromString(static_cast<char*>(buffer)); buffer = nullptr;
      if (token->classID() == Guid::null()) {
         delete token; token = nullptr;
      }
      m_inputStreamingTool->getObject(&buffer, nbytes).ignore();
   } else {
      token = m_poolSvc->getToken(par[0], par[1], ip[0]);
   }
   if (token == nullptr) {
      return(StatusCode::RECOVERABLE);
   }
   refpAddress = new TokenAddress(POOL_StorageType, clid, "", par[1], IPoolSvc::kInputStream, token);
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::createAddress(long svcType,
		const CLID& clid,
		const std::string& refAddress,
		IOpaqueAddress*& refpAddress) {
   if (svcType != POOL_StorageType) {
      ATH_MSG_ERROR("createAddress: svcType != POOL_StorageType " << svcType << " " << POOL_StorageType);
      return(StatusCode::FAILURE);
   }
   refpAddress = new GenericAddress(POOL_StorageType, clid, refAddress);
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::convertAddress(const IOpaqueAddress* pAddress,
		std::string& refAddress) {
   assert(pAddress);
   const TokenAddress* tokAddr = dynamic_cast<const TokenAddress*>(pAddress);
   if (tokAddr != nullptr && tokAddr->getToken() != nullptr) {
      refAddress = tokAddr->getToken()->toString();
   } else {
      refAddress = *pAddress->par();
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaPoolCnvSvc::decodeOutputSpec(std::string& fileSpec,
		int& outputTech) const {
  if (boost::starts_with (fileSpec, "oracle") || boost::starts_with (fileSpec, "mysql")) {
      outputTech = pool::POOL_RDBMS_StorageType.type();
   } else if (boost::starts_with (fileSpec, "ROOTKEY:")) {
      outputTech = pool::ROOTKEY_StorageType.type();
      fileSpec.erase(0, 8);
   } else if (boost::starts_with (fileSpec, "ROOTTREE:")) {
      outputTech = pool::ROOTTREE_StorageType.type();
      fileSpec.erase(0, 9);
   } else if (boost::starts_with (fileSpec, "ROOTTREEINDEX:")) {
      outputTech = pool::ROOTTREEINDEX_StorageType.type();
      fileSpec.erase(0, 14);
   } else if (outputTech == 0) {
      outputTech = pool::ROOTTREEINDEX_StorageType.type();
   }
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::registerCleanUp(IAthenaPoolCleanUp* cnv) {
   m_cnvs.push_back(cnv);
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::cleanUp(const std::string& connection) {
   bool retError = false;
   std::size_t cpos = connection.find(":");
   std::size_t bpos = connection.find("[");
   if (cpos == std::string::npos) {
      cpos = 0;
   } else {
      cpos++;
   }
   if (bpos != std::string::npos) bpos = bpos - cpos;
   for (auto convertr : m_cnvs) {
      if (!convertr->cleanUp(connection.substr(cpos, bpos)).isSuccess()) {
         ATH_MSG_WARNING("AthenaPoolConverter cleanUp failed.");
         retError = true;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::setInputAttributes(const std::string& fileName) {
   // Set attributes for input file
   m_lastInputFileName = fileName; // Save file name for printing attributes per event
   if (!processPoolAttributes(m_inputAttr, m_lastInputFileName, IPoolSvc::kInputStream, false, true, false).isSuccess()) {
      ATH_MSG_DEBUG("setInputAttribute failed setting POOL database/container attributes.");
   }
   if (!processPoolAttributes(m_inputAttr, m_lastInputFileName, IPoolSvc::kInputStream, true, false).isSuccess()) {
      ATH_MSG_DEBUG("setInputAttribute failed getting POOL database/container attributes.");
   }
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::makeServer(int num) {
   if (num < 0) {
      num = -num;
      m_streamServer = int(num / 1024);
      num = num % 1024;
      if (!m_outputStreamingTool.empty() && m_streamServer < m_outputStreamingTool.size()
		      && !m_outputStreamingTool[m_streamServer]->isServer()) {
         ATH_MSG_DEBUG("makeServer: " << m_outputStreamingTool << " = " << num);
         if (m_outputStreamingTool[m_streamServer]->makeServer(num).isFailure()) {
            ATH_MSG_ERROR("makeServer: " << m_outputStreamingTool << " failed");
            return(StatusCode::FAILURE);
         }
         m_streamClientFiles.clear();
         return(StatusCode::SUCCESS);
      }
      return(StatusCode::RECOVERABLE);
   }
   if (m_inputStreamingTool.empty()) {
      return(StatusCode::RECOVERABLE);
   }
   ATH_MSG_DEBUG("makeServer: " << m_inputStreamingTool << " = " << num);
   return(m_inputStreamingTool->makeServer(num));
}
//________________________________________________________________________________
StatusCode AthenaPoolCnvSvc::makeClient(int num) {
   if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isClient() && num > 0) {
      ATH_MSG_DEBUG("makeClient: " << m_outputStreamingTool << " = " << num);
      for (std::size_t streamClient = 0; streamClient < m_outputStreamingTool.size(); streamClient++) {
         if (m_outputStreamingTool[streamClient]->makeClient(num).isFailure()) {
            ATH_MSG_ERROR("makeClient: " << m_outputStreamingTool << ", " << streamClient << " failed");
            return(StatusCode::FAILURE);
         }
      }
   }
   if (m_inputStreamingTool.empty()) {
      return(StatusCode::SUCCESS);
   }
   ATH_MSG_DEBUG("makeClient: " << m_inputStreamingTool << " = " << num);
   return(m_inputStreamingTool->makeClient(num));
}
//________________________________________________________________________________
StatusCode AthenaPoolCnvSvc::readData() const {
   if (m_inputStreamingTool.empty()) {
      return(StatusCode::FAILURE);
   }
   char* tokenStr = nullptr;
   int num = -1;
   StatusCode sc = m_inputStreamingTool->clearObject(&tokenStr, num);
   if (sc.isSuccess() && tokenStr != nullptr && strlen(tokenStr) > 0 && num > 0) {
      ATH_MSG_DEBUG("readData: " << tokenStr << ", for client: " << num);
   } else {
      return(sc);
   }
   // Read object instance via POOL/ROOT
   void* instance = nullptr;
   Token token;
   token.fromString(tokenStr); tokenStr = nullptr;
   if (token.classID() != Guid::null()) {
      std::string objName = "ALL";
      if (m_useDetailChronoStat.value()) {
         objName = token.classID().toString();
      }
      if (m_doChronoStat) {
         m_chronoStatSvc->chronoStart("cObj_" + objName);
      }
      this->setObjPtr(instance, &token);
      // Serialize object via ROOT
      RootType cltype(pool::DbReflex::forGuid(token.classID()));
      void* buffer = nullptr;
      std::size_t nbytes = 0;
      buffer = m_serializeSvc->serialize(instance, cltype, nbytes);
      sc = m_inputStreamingTool->putObject(buffer, nbytes, num);
      while (sc.isRecoverable()) {
         usleep(100);
         sc = m_inputStreamingTool->putObject(buffer, nbytes, num);
      }
      delete [] static_cast<char*>(buffer); buffer = nullptr;
      if (!sc.isSuccess()) {
         ATH_MSG_ERROR("Could not share object for: " << token.toString());
         return(StatusCode::FAILURE);
      }
      AuxDiscoverySvc auxDiscover;
      if (!auxDiscover.sendStore(m_serializeSvc.get(), m_inputStreamingTool.get(), instance, token.classID(), token.contID(), num).isSuccess()) {
         ATH_MSG_ERROR("Could not share dynamic aux store for: " << token.toString());
         return(StatusCode::FAILURE);
      }
      cltype.Destruct(instance); instance = nullptr;
      if (!m_inputStreamingTool->putObject(nullptr, 0, num).isSuccess()) {
         ATH_MSG_ERROR("Could not share object for: " << token.toString());
         return(StatusCode::FAILURE);
      }
      if (m_doChronoStat) {
         m_chronoStatSvc->chronoStop("cObj_" + objName);
      }
   } else if (token.dbID() != Guid::null()) {
      std::string returnToken;
      const Token* metadataToken = m_poolSvc->getToken("FID:" + token.dbID().toString(), token.contID(), token.oid().first);
      if (metadataToken != nullptr) {
         returnToken = metadataToken->toString();
      } else {
         returnToken = token.toString();
      }
      delete metadataToken; metadataToken = nullptr;
      // Share token
      sc = m_inputStreamingTool->putObject(returnToken.c_str(), returnToken.size() + 1, num);
      if (!sc.isSuccess() || !m_inputStreamingTool->putObject(nullptr, 0, num).isSuccess()) {
         ATH_MSG_ERROR("Could not share token for: " << token.toString());
         return(StatusCode::FAILURE);
      }
   } else {
      return(StatusCode::RECOVERABLE);
   }
   return(StatusCode::SUCCESS);
}

//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::abortSharedWrClients(int client_n)
{
   ATH_MSG_ERROR("Sending ABORT to clients");
   // the master process will kill this process once workers abort
   // but it could be a time-limited loop
   auto streamingTool = m_outputStreamingTool[m_streamServer];
   StatusCode sc = StatusCode::SUCCESS;
   while (sc.isSuccess()) {
      if (client_n >= 0) {
         sc = streamingTool->lockObject("ABORT", client_n);
      }
      char* dummy;
      sc = streamingTool->clearObject(&dummy, client_n);
      while (sc.isRecoverable()) {
         sc = streamingTool->clearObject(&dummy, client_n);
      }
   }
   return StatusCode::FAILURE;
}

//______________________________________________________________________________
void AthenaPoolCnvSvc::handle(const Incident& incident) {
   if (incident.type() == "EndEvent") {
      if (!processPoolAttributes(m_inputAttrPerEvent, m_lastInputFileName, IPoolSvc::kInputStream).isSuccess()) {
         ATH_MSG_DEBUG("handle EndEvent failed process POOL database attributes.");
      }
   }
}
//______________________________________________________________________________
AthenaPoolCnvSvc::AthenaPoolCnvSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthCnvSvc(name, pSvcLocator, POOL_StorageType),
	m_outputStreamingTool(this)
{
   declareProperty("OutputStreamingTool", m_outputStreamingTool);
}
//______________________________________________________________________________
AthenaPoolCnvSvc::~AthenaPoolCnvSvc() {
}
//__________________________________________________________________________
void AthenaPoolCnvSvc::extractPoolAttributes(const StringArrayProperty& property,
		std::vector<std::vector<std::string> >* contAttr,
		std::vector<std::vector<std::string> >* dbAttr,
		std::vector<std::vector<std::string> >* domAttr) const {
   std::vector<std::string> opt;
   std::string attributeName, containerName, databaseName, valueString;
   for (std::vector<std::string>::const_iterator iter = property.value().begin(),
           last = property.value().end(); iter != last; ++iter) {
      opt.clear();
      attributeName.clear();
      containerName.clear();
      databaseName.clear();
      valueString.clear();
      using Gaudi::Utils::AttribStringParser;
      for (const AttribStringParser::Attrib& attrib : AttribStringParser (*iter)) {
         const std::string tag = attrib.tag;
         const std::string val = attrib.value;
         if (tag == "DatabaseName") {
            databaseName = val;
         } else if (tag == "ContainerName") {
            if (databaseName.empty()) {
               databaseName = "*";
            }
            containerName = val;
         } else {
            attributeName = tag;
            valueString = val;
         }
      }
      if (!attributeName.empty() && !valueString.empty()) {
         opt.push_back(attributeName);
         opt.push_back(valueString);
         if (!databaseName.empty()) {
            opt.push_back(databaseName);
            if (!containerName.empty()) {
               opt.push_back(containerName);
               if (containerName.substr(0, 6) == "TTree=") {
                  dbAttr->push_back(opt);
               } else {
                  contAttr->push_back(opt);
               }
            } else {
               opt.push_back("");
               dbAttr->push_back(opt);
            }
         } else if (domAttr != 0) {
            domAttr->push_back(opt);
         } else {
            opt.push_back("*");
            opt.push_back("");
            dbAttr->push_back(opt);
         }
      }
   }
}
//__________________________________________________________________________
StatusCode AthenaPoolCnvSvc::processPoolAttributes(std::vector<std::vector<std::string> >& attr,
		const std::string& fileName,
		unsigned long contextId,
		bool doGet,
		bool doSet,
		bool doClear) const {
   bool retError = false;
   if (!m_inputStreamingTool.empty() && m_inputStreamingTool->isClient()) doGet = false;
   for (std::vector<std::vector<std::string> >::iterator iter = attr.begin(), last = attr.end();
		   iter != last; ++iter) {
      if (iter->size() == 2) {
         const std::string& opt = (*iter)[0];
         std::string data = (*iter)[1];
         if (data == "int" || data == "DbLonglong" || data == "double" || data == "string") {
            if (doGet) {
               if (!m_poolSvc->getAttribute(opt, data, pool::DbType(pool::ROOTTREE_StorageType).type(), contextId).isSuccess()) {
                  ATH_MSG_DEBUG("getAttribute failed for domain attr " << opt);
                  retError = true;
               }
            }
         } else if (doSet) {
            if (m_poolSvc->setAttribute(opt, data, pool::DbType(pool::ROOTTREE_StorageType).type(), contextId).isSuccess()) {
               ATH_MSG_DEBUG("setAttribute " << opt << " to " << data);
               if (doClear) {
                  iter->clear();
               }
            } else {
               ATH_MSG_DEBUG("setAttribute failed for domain attr " << opt << " to " << data);
               retError = true;
            }
         }
      }
      if (iter->size() == 4) {
         const std::string& opt = (*iter)[0];
         std::string data = (*iter)[1];
         const std::string& file = (*iter)[2];
         const std::string& cont = (*iter)[3];
         if (!fileName.empty() && (file == fileName || (file.substr(0, 1) == "*"
		         && file.find("," + fileName + ",") == std::string::npos))) {
            if (data == "int" || data == "DbLonglong" || data == "double" || data == "string") {
               if (doGet) {
                  if (!m_poolSvc->getAttribute(opt, data, pool::DbType(pool::ROOTTREE_StorageType).type(), fileName, cont, contextId).isSuccess()) {
                     ATH_MSG_DEBUG("getAttribute failed for database/container attr " << opt);
                     retError = true;
                  }
               }
            } else if (doSet) {
               if (m_poolSvc->setAttribute(opt, data, pool::DbType(pool::ROOTTREE_StorageType).type(), fileName, cont, contextId).isSuccess()) {
                  ATH_MSG_DEBUG("setAttribute " << opt << " to " << data << " for db: " << fileName << " and cont: " << cont);
                  if (doClear) {
                     if (file.substr(0, 1) == "*" && !m_persSvcPerOutput) {
                        (*iter)[2] += "," + fileName + ",";
                     } else {
                        iter->clear();
                     }
                  }
               } else {
                  ATH_MSG_DEBUG("setAttribute failed for " << opt << " to " << data << " for db: " << fileName << " and cont: " << cont);
                  retError = true;
               }
            }
         }
      }
   }
   for (std::vector<std::vector<std::string> >::iterator iter = attr.begin(); iter != attr.end(); ) {
      if (iter->empty()) {
         iter = attr.erase(iter);
      } else {
         ++iter;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
