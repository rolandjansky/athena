/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaPoolCnvSvc.cxx
 *  @brief This file contains the implementation for the AthenaPoolCnvSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaPoolCnvSvc.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/FileIncident.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/AttribStringParser.h"

#include "AthenaKernel/IAthenaIPCTool.h"
#include "AthenaKernel/IAthenaSerializeSvc.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"
#include "PersistentDataModel/TokenAddress.h"
#include "PersistentDataModel/DataHeader.h"
#include "PoolSvc/IPoolSvc.h"

#include "StorageSvc/DbReflex.h"

#include "AuxDiscoverySvc.h"

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
	   last = m_maxFileSizes.value().end(); iter != last; iter++) {
      if (iter->find('=') != std::string::npos) {
         long long maxFileSize = atoll(iter->substr(iter->find('=') + 1).c_str());
         if (maxFileSize > 10000000000LL) {
            ATH_MSG_WARNING("Files larger than 10GB are disallowed by ATLAS policy.");
            ATH_MSG_WARNING("They should only be produced for private use or in special cases.");
         }
         std::string databaseName = iter->substr(0, iter->find_first_of(" 	="));
         std::pair<std::string, long long> entry(databaseName, maxFileSize);
         m_databaseMaxFileSize.insert(entry);
      } else {
         m_domainMaxFileSize = atoll(iter->c_str());
         if (m_domainMaxFileSize > 10000000000LL) {
            ATH_MSG_WARNING("Files larger than 10GB are disallowed by ATLAS policy.");
            ATH_MSG_WARNING("They should only be produced for private use or in special cases.");
         }
      }
   }
   // Setting default 'TREE_MAX_SIZE' for ROOT to 1024 GB to avoid file chains.
   std::vector<std::string> maxFileSize;
   maxFileSize.push_back("TREE_MAX_SIZE");
   maxFileSize.push_back("1099511627776L");
   m_domainAttr.push_back(maxFileSize);
   // Extracting OUTPUT POOL ItechnologySpecificAttributes for Domain, Database and Container.
   extractPoolAttributes(m_poolAttr, &m_containerAttr, &m_databaseAttr, &m_domainAttr);
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
   pool::DbType dbType = m_dbType;
   m_dbType = pool::DbType(pool::ROOTTREE_StorageType);
   if (!processPoolAttributes(m_inputAttr, "", IPoolSvc::kInputStream, false, true, true).isSuccess()) {
      ATH_MSG_DEBUG("setInputAttribute failed setting POOL domain attributes.");
   }
   m_dbType = dbType;
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
      try {
         // Forward to base class fillRepRefs
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
   // Reset streaming parameters to CnvSvc properties.
   m_dhContainerPrefix = "POOLContainer";
   m_containerPrefix = m_containerPrefixProp.value();
   m_containerNameHint = m_containerNameHintProp.value();
   m_branchNameHint = m_branchNameHintProp.value();

   // Override streaming parameters from StreamTool if requested.
   std::string::size_type pos1 = outputConnectionSpec.find("[");
   m_outputConnectionSpec = outputConnectionSpec.substr(0, pos1);
   while (pos1 != std::string::npos) {
      const std::string::size_type pos2 = outputConnectionSpec.find("=", pos1);
      const std::string key = outputConnectionSpec.substr(pos1 + 1, pos2 - pos1 - 1);
      const std::string::size_type pos3 = outputConnectionSpec.find("]", pos2);
      const std::string value = outputConnectionSpec.substr(pos2 + 1, pos3 - pos2 - 1);
      if (key == "OutputCollection") {
         m_dhContainerPrefix = value;
      } else if (key == "PoolContainerPrefix") {
         m_containerPrefix = value;
      } else if (key == "TopLevelContainerName") {
         m_containerNameHint = value;
      } else if (key == "SubLevelBranchName") {
         m_branchNameHint = value;
      }
      pos1 = outputConnectionSpec.find("[", pos3);
   }

   // Extract the technology
   if (!decodeOutputSpec(m_outputConnectionSpec, m_dbType).isSuccess()) {
      ATH_MSG_ERROR("connectOutput FAILED extract file name and technology.");
      return(StatusCode::FAILURE);
   }

   if (!m_outputStreamingTool.empty() && m_outputStreamingTool[0]->isClient()) {
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty()
		   && (m_streamServer == m_outputStreamingTool.size() || !m_outputStreamingTool[0]->isServer())) {
      ATH_MSG_DEBUG("connectOutput SKIPPED for expired server.");
      return(StatusCode::SUCCESS);
   }
   try {
      if (!m_poolSvc->connect(pool::ITransaction::UPDATE).isSuccess()) {
         ATH_MSG_ERROR("connectOutput FAILED to open an UPDATE transaction.");
         return(StatusCode::FAILURE);
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("connectOutput - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   if (!processPoolAttributes(m_domainAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream).isSuccess()) {
      ATH_MSG_DEBUG("connectOutput failed process POOL domain attributes.");
   }
   if (!processPoolAttributes(m_databaseAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream).isSuccess()) {
      ATH_MSG_DEBUG("connectOutput failed process POOL database attributes.");
   }
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::commitOutput(const std::string& outputConnectionSpec, bool doCommit) {
// This is called after all DataObjects are converted.
   if (!m_outputStreamingTool.empty() && m_outputStreamingTool[0]->isClient()) {
      std::size_t streamClient = 0;
      for (std::vector<std::string>::const_iterator iter = m_streamClientFiles.begin(), last = m_streamClientFiles.end(); iter != last; iter++) {
         if (*iter == outputConnectionSpec) break;
         streamClient++;
      }
      if (streamClient == m_streamClientFiles.size()) {
         if (m_streamClientFiles.size() < m_outputStreamingTool.size()) {
            m_streamClientFiles.push_back(outputConnectionSpec);
         } else {
            streamClient = 0;
         }
      }
      StatusCode sc = m_outputStreamingTool[streamClient]->lockObject("release");
      while (sc.isRecoverable()) {
         usleep(100);
         sc = m_outputStreamingTool[streamClient]->lockObject("release");
      }
      if (!this->cleanUp().isSuccess()) {
         ATH_MSG_ERROR("commitOutput FAILED to cleanup converters.");
         return(StatusCode::FAILURE);
      }
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty() && m_metadataContainerProp.value().empty()
                  && m_streamServer == m_outputStreamingTool.size()) {
      ATH_MSG_DEBUG("commitOutput SKIPPED for expired server.");
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isServer()) {
      ATH_MSG_DEBUG("commitOutput SKIPPED for uninitialized server.");
      return(StatusCode::SUCCESS);
   }
   std::map<void*, RootType> commitCache;
   if (!m_outputStreamingTool.empty() && m_streamServer < m_outputStreamingTool.size()
		   && m_outputStreamingTool[m_streamServer]->isServer()) {
      // Clear object to get Placements for all objects in a Stream
      char* placementStr = nullptr;
      int num = -1;
      StatusCode sc = m_outputStreamingTool[m_streamServer]->clearObject(&placementStr, num);
      if (sc.isSuccess() && placementStr != nullptr && strlen(placementStr) > 0 && num > 0) {
         std::string fileName = strstr(placementStr, "[FILE=");
         fileName = fileName.substr(6, fileName.find(']') - 6);
         if (!this->connectOutput(fileName).isSuccess()) {
            ATH_MSG_ERROR("Failed to connectOutput for " << fileName);
            return(StatusCode::FAILURE);
         }
         while (strncmp(placementStr, "release", 7) != 0) {
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
            RootType classDesc = RootType::ByName(className);
            void* obj = nullptr;
            std::string::size_type len = m_metadataContainerProp.value().size();
            // For Metadata fire incident to read object into store
            if (len > 0 && contName.substr(0, len) == m_metadataContainerProp.value()
		            && contName.substr(len, 1) == "(") {
               ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
               std::ostringstream oss;
               oss << std::dec << num;
               FileIncident proxyIncident(name(), "ShmProxy", std::string(placementStr) + "[NUM=" + oss.str() + "]");
               incSvc->fireIncident(proxyIncident); // Object will be pulled out of shared memory by setObjPtr()
            } else {
               Token readToken;
               readToken.setOid(Token::OID_t(num, 0));
               readToken.setAuxString("[PNAME=" + className + "]");
               this->setObjPtr(obj, &readToken); // Pull/read Obbject out of shared memory
               if (len > 0 && contName.substr(0, len) == m_metadataContainerProp.value()) {
                  ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", name());
                  if (contName.substr(len, 7) == "HdrForm") {
                     FileIncident beginInputIncident(name(), "BeginInputFile", "SHM");
                     incSvc->fireIncident(beginInputIncident);
                  } else if (contName.substr(len, 3) == "Hdr") {
                     FileIncident endInputIncident(name(), "EndInputFile", "SHM");
                     incSvc->fireIncident(endInputIncident);
                  }
               } else {
                  // Write object
                  Placement placement;
                  placement.fromString(placementStr); placementStr = nullptr;
                  const Token* token = this->registerForWrite(&placement, obj, classDesc);
                  if (token == nullptr) {
                     ATH_MSG_ERROR("Failed to write Data for: " << className);
                     return(StatusCode::FAILURE);
                  }
                  tokenStr = token->toString();
                  delete token; token = nullptr;

                  // For DataHeaderForm, Token needs to be inserted to DataHeader Object
                  if (className == "DataHeaderForm_p5") {
                     GenericAddress address(POOL_StorageType, ClassID_traits<DataHeader>::ID(), tokenStr, placement.auxString());
                     IConverter* cnv = converter(ClassID_traits<DataHeader>::ID());
                     if (!cnv->updateRepRefs(&address, static_cast<DataObject*>(obj)).isSuccess()) {
                        ATH_MSG_ERROR("Failed updateRepRefs for obj = " << tokenStr);
                        return(StatusCode::FAILURE);
                     }
                  // Found DataHeader
                  } else if (className == "DataHeader_p5") {
                     GenericAddress address(POOL_StorageType, ClassID_traits<DataHeader>::ID(), tokenStr, placement.auxString());
                     IConverter* cnv = converter(ClassID_traits<DataHeader>::ID());
                     if (!cnv->updateRep(&address, static_cast<DataObject*>(obj)).isSuccess()) {
                        ATH_MSG_ERROR("Failed updateRep for obj = " << tokenStr);
                        return(StatusCode::FAILURE);
                     }
                     commitCache.insert(std::pair<void*, RootType>(obj, classDesc));
                  } else if (className != "Token" && !classDesc.IsFundamental()) {
                     commitCache.insert(std::pair<void*, RootType>(obj, classDesc));
                  }

               }
            }

            // Send Token back to Client
            sc = m_outputStreamingTool[m_streamServer]->lockObject(tokenStr.c_str(), num);
            if (!sc.isSuccess()) {
               ATH_MSG_ERROR("Failed to lock Data for " << tokenStr);
               return(StatusCode::FAILURE);
            }
            sc = m_outputStreamingTool[m_streamServer]->clearObject(&placementStr, num);
            while (sc.isRecoverable()) {
               sc = m_outputStreamingTool[m_streamServer]->clearObject(&placementStr, num);
            }
            if (!sc.isSuccess()) {
               ATH_MSG_ERROR("Failed to get Data for client: " << num);
               return(StatusCode::FAILURE);
            }
            if (m_doChronoStat) {
               m_chronoStatSvc->chronoStop("cRep_" + objName);
            }
         }
         placementStr = nullptr;
      } else if (sc.isRecoverable() || num == -1) {
         return(StatusCode::RECOVERABLE);
      } else {
         ATH_MSG_INFO("Failed to get Data for client: " << num);
         return(StatusCode::FAILURE);
      }
   }
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStart("commitOutput");
   }
   if (!processPoolAttributes(m_domainAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream).isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL domain attributes.");
   }
   if (!processPoolAttributes(m_databaseAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream).isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL database attributes.");
   }
   if (!processPoolAttributes(m_containerAttr, m_outputConnectionSpec, IPoolSvc::kOutputStream).isSuccess()) {
      ATH_MSG_DEBUG("commitOutput failed process POOL container attributes.");
   }
   try {
      if (doCommit) {
         if (!m_poolSvc->commit(IPoolSvc::kOutputStream).isSuccess()) {
            ATH_MSG_ERROR("commitOutput FAILED to commit OutputStream.");
            return(StatusCode::FAILURE);
         }
      } else {
         if (!m_poolSvc->commitAndHold(IPoolSvc::kOutputStream).isSuccess()) {
            ATH_MSG_ERROR("commitOutput FAILED to commitAndHold OutputStream.");
            return(StatusCode::FAILURE);
         }
      }
   } catch (std::exception& e) {
      ATH_MSG_ERROR("commitOutput - caught exception: " << e.what());
      return(StatusCode::FAILURE);
   }
   if (!this->cleanUp().isSuccess()) {
      ATH_MSG_ERROR("commitOutput FAILED to cleanup converters.");
      return(StatusCode::FAILURE);
   }
   for (std::map<void*, RootType>::iterator iter = commitCache.begin(), last = commitCache.end(); iter != last; iter++) {
      iter->second.Destruct(iter->first);
   }
   // Check FileSize
   long long int currentFileSize = m_poolSvc->getFileSize(m_outputConnectionSpec, m_dbType.type(), IPoolSvc::kOutputStream);
   if (m_databaseMaxFileSize.find(m_outputConnectionSpec) != m_databaseMaxFileSize.end()) {
      if (currentFileSize > m_databaseMaxFileSize[m_outputConnectionSpec]) {
         ATH_MSG_WARNING("FileSize > MaxFileSize for " << m_outputConnectionSpec);
         return(StatusCode::RECOVERABLE);
      }
   } else if (currentFileSize > m_domainMaxFileSize) {
      ATH_MSG_WARNING("FileSize > domMaxFileSize for " << m_outputConnectionSpec);
      return(StatusCode::RECOVERABLE);
   }
   // For "safety" we reset the output file and the technology type
   m_outputConnectionSpec = "";
   m_dbType = pool::DbType();
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStop("commitOutput");
   }
   // Prepare chrono for next commit
   m_doChronoStat = true;
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::disconnectOutput() {
   if (!m_outputStreamingTool.empty() && m_outputStreamingTool[0]->isClient()) {
      return(StatusCode::SUCCESS);
   }
   if (!m_outputStreamingTool.empty()) {
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
   // Setting default 'TREE_MAX_SIZE' for ROOT to 1024 GB to avoid file chains.
   std::vector<std::string> maxFileSize;
   maxFileSize.push_back("TREE_MAX_SIZE");
   maxFileSize.push_back("1099511627776L");
   m_domainAttr.push_back(maxFileSize);
   // Extracting OUTPUT POOL ItechnologySpecificAttributes for Domain, Database and Container.
   extractPoolAttributes(m_poolAttr, &m_containerAttr, &m_databaseAttr, &m_domainAttr);
   return(m_poolSvc->disconnect(IPoolSvc::kOutputStream));
}
//______________________________________________________________________________
const std::string& AthenaPoolCnvSvc::getOutputConnectionSpec() const {
   return(m_outputConnectionSpec);
}
//______________________________________________________________________________
std::string AthenaPoolCnvSvc::getOutputContainer(const std::string& typeName,
		const std::string& key) const {
   if (typeName.substr(0, 14) == "DataHeaderForm") {
      return(m_dhContainerPrefix + "Form" + "(" + typeName + ")");
   }
   if (typeName.substr(0, 10) == "DataHeader") {
      if (key.substr(key.size() - 1) == "/") {
         return(m_dhContainerPrefix + "(" + key + typeName + ")");
      }
      return(m_dhContainerPrefix + "(" + typeName + ")");
   }
   if (typeName.substr(0, 13) == "AttributeList") {
      return(m_collContainerPrefix + "(" + key + ")");
   }
   if (key.empty()) {
      return(m_containerPrefix + typeName);
   }
   const std::string typeTok = "<type>", keyTok = "<key>";
   std::string ret = m_containerPrefix + m_containerNameHint;
   if (!m_branchNameHint.empty()) {
      ret += "(" + m_branchNameHint + ")";
   }
   const std::size_t pos1 = ret.find(typeTok);
   if (pos1 != std::string::npos) {
      ret.replace(pos1, typeTok.size(), typeName);
   }
   const std::size_t pos2 = ret.find(keyTok);
   if (pos2 != std::string::npos) {
      if (key.empty()) {
         ret.replace(pos2, keyTok.size(), typeName);
      } else {
         ret.replace(pos2, keyTok.size(), key);
      }
   }
   return(ret);
}
//______________________________________________________________________________
pool::DbType AthenaPoolCnvSvc::technologyType() const {
   return(m_dbType);
}
//__________________________________________________________________________
IPoolSvc* AthenaPoolCnvSvc::getPoolSvc() {
   return(&*m_poolSvc);
}
//______________________________________________________________________________
const Token* AthenaPoolCnvSvc::registerForWrite(const Placement* placement,
		const void* obj,
		const RootType& classDesc) const {
   if (m_doChronoStat) {
      m_chronoStatSvc->chronoStart("cRepR_ALL");
   }
   const Token* token = nullptr;
   if (!m_outputStreamingTool.empty() && m_outputStreamingTool[0]->isClient()) {
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
      std::string placementStr = placement->toString();
      std::size_t formPos = placementStr.find("[FORM=");
      if (formPos != std::string::npos) {
         placementStr = placementStr.substr(0, formPos) + "[PNAME=" + classDesc.Name() + "]" + placementStr.substr(formPos);
      } else {
         placementStr += "[PNAME=" + classDesc.Name() + "]";
      }
      ATH_MSG_VERBOSE("Requesting write object for: " << placementStr);
      StatusCode sc = m_outputStreamingTool[streamClient]->lockObject(placementStr.c_str());
      while (sc.isRecoverable()) {
         usleep(100);
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
         usleep(100);
         sc = m_outputStreamingTool[streamClient]->putObject(buffer, nbytes);
      }
      if (own) { delete [] static_cast<const char*>(buffer); }
      buffer = nullptr;
      AuxDiscoverySvc auxDiscover;
      if (!auxDiscover.sendStore(m_serializeSvc.get(), m_outputStreamingTool[streamClient].get(), obj, pool::DbReflex::guid(classDesc), placement->containerName()).isSuccess()) {
         ATH_MSG_ERROR("Could not share dynamic aux store for: " << placementStr);
         return(nullptr);
      }
      if (!sc.isSuccess() || !m_outputStreamingTool[streamClient]->putObject(nullptr, 0).isSuccess()) {
         ATH_MSG_ERROR("Failed to put Data for " << placementStr);
         return(nullptr);
      }
      // Get Token back from Server
      char* tokenStr = nullptr;
      int num = -1;
      sc = m_outputStreamingTool[streamClient]->clearObject(&tokenStr, num);
      while (sc.isRecoverable()) {
         usleep(100);
         sc = m_outputStreamingTool[streamClient]->clearObject(&tokenStr, num);
      }
      if (!sc.isSuccess()) {
         ATH_MSG_ERROR("Failed to get Token");
         return(nullptr);
      }
      Token* tempToken = new Token();
      tempToken->fromString(tokenStr); tokenStr = nullptr;
      tempToken->setClassID(pool::DbReflex::guid(classDesc));
      token = tempToken; tempToken = nullptr;
   } else {
      if (!m_outputStreamingTool.empty() && m_metadataContainerProp.value().empty()
		      && (m_streamServer == m_outputStreamingTool.size() || !m_outputStreamingTool[0]->isServer())) {
         ATH_MSG_DEBUG("registerForWrite SKIPPED for expired server, Placement = " << placement->toString());
         Token* tempToken = new Token();
         tempToken->setClassID(pool::DbReflex::guid(classDesc));
         token = tempToken; tempToken = nullptr;
      } else if (!m_outputStreamingTool.empty() && !m_outputStreamingTool[0]->isServer()) {
         ATH_MSG_DEBUG("registerForWrite SKIPPED for uninitialized server, Placement = " << placement->toString());
         Token* tempToken = new Token();
         tempToken->setClassID(pool::DbReflex::guid(classDesc));
         token = tempToken; tempToken = nullptr;
      } else {
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
            return;
         }
         if (token->classID() != Guid::null()) {
            // Deserialize object
            RootType cltype(pool::DbReflex::forGuid(token->classID()));
            obj = m_serializeSvc->deserialize(buffer, nbytes, cltype); buffer = nullptr;
         } else {
            // Deserialize object
            std::string className = token->auxString();
            className = className.substr(className.find("[PNAME="));
            className = className.substr(7, className.find(']') - 7);
            RootType cltype(RootType::ByName(className));
            obj = m_serializeSvc->deserialize(buffer, nbytes, cltype); buffer = nullptr;
         }
         AuxDiscoverySvc auxDiscover;
         if (!auxDiscover.receiveStore(m_serializeSvc.get(), m_outputStreamingTool[m_streamServer].get(), obj, num).isSuccess()) {
            ATH_MSG_ERROR("Failed to get Dynamic Aux Store for " << token->toString());
         }
         return;
      }
   }
   if (!m_inputStreamingTool.empty() && m_inputStreamingTool->isClient()) {
      ATH_MSG_VERBOSE("Requesting object for: " << token->toString());
      if (!m_inputStreamingTool->lockObject(token->toString().c_str()).isSuccess()) {
         ATH_MSG_WARNING("Failed to lock Data for " << token->toString());
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
            ATH_MSG_WARNING("Failed to get Data for " << token->toString());
            obj = nullptr;
         } else {
            obj = m_serializeSvc->deserialize(buffer, nbytes, token->classID()); buffer = nullptr;
            AuxDiscoverySvc auxDiscover;
            if (!auxDiscover.receiveStore(m_serializeSvc.get(), m_inputStreamingTool.get(), obj).isSuccess()) {
               ATH_MSG_ERROR("Failed to get Dynamic Aux Store for " << token->toString());
            }
         }
      }
   } else {
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
   if (par[0] == "SHM") {
      token = new Token();
      token->setOid(Token::OID_t(ip[0], ip[1]));
      token->setAuxString("[PNAME=" + par[2] + "]");
      RootType classDesc = RootType::ByName(par[2]);
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
   refpAddress = new TokenAddress(POOL_StorageType, clid, "", par[1], ip[0], token);
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
   refAddress = *pAddress->par();
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::registerCleanUp(IAthenaPoolCleanUp* cnv) {
   m_cnvs.push_back(cnv);
   return(StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::cleanUp() {
   bool retError = false;
   for (std::vector<IAthenaPoolCleanUp*>::iterator iter = m_cnvs.begin(), last = m_cnvs.end();
		   iter != last; iter++) {
      if (!(*iter)->cleanUp().isSuccess()) {
         ATH_MSG_WARNING("AthenaPoolConverter cleanUp failed.");
         retError = true;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
//______________________________________________________________________________
StatusCode AthenaPoolCnvSvc::setInputAttributes(const std::string& fileName) {
   // Set attributes for input file
   m_lastFileName = fileName; // Save file name for printing attributes per event
   pool::DbType dbType = m_dbType;
   m_dbType = pool::DbType(pool::ROOTTREE_StorageType);
   if (!processPoolAttributes(m_inputAttr, m_lastFileName, IPoolSvc::kInputStream, false, true, false).isSuccess()) {
      ATH_MSG_DEBUG("setInputAttribute failed setting POOL database/container attributes.");
   }
   if (!processPoolAttributes(m_inputAttr, m_lastFileName, IPoolSvc::kInputStream, true, false).isSuccess()) {
      ATH_MSG_DEBUG("setInputAttribute failed getting POOL database/container attributes.");
   }
   m_dbType = dbType;
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
void AthenaPoolCnvSvc::handle(const Incident& incident) {
   if (incident.type() == "EndEvent") {
      pool::DbType dbType = m_dbType;
      m_dbType = pool::DbType(pool::ROOTTREE_StorageType);
      if (!processPoolAttributes(m_inputAttrPerEvent, m_lastFileName, IPoolSvc::kInputStream).isSuccess()) {
         ATH_MSG_DEBUG("handle EndEvent failed process POOL database attributes.");
      }
      m_dbType = dbType;
   }
}
//______________________________________________________________________________
AthenaPoolCnvSvc::AthenaPoolCnvSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	::AthCnvSvc(name, pSvcLocator, POOL_StorageType),
	m_dbType(),
	m_outputConnectionSpec(),
	m_dhContainerPrefix("POOLContainer"),
	m_collContainerPrefix("POOLCollectionTree"),
	m_lastFileName(),
	m_poolSvc("PoolSvc", name),
	m_chronoStatSvc("ChronoStatSvc", name),
	m_clidSvc("ClassIDSvc", name),
	m_serializeSvc("AthenaRootSerializeSvc", name),
	m_inputStreamingTool("", this),
	m_outputStreamingTool(this),
	m_streamServer(0),
	m_containerPrefix(),
	m_containerNameHint(),
	m_branchNameHint(),
	m_domainMaxFileSize(10000000000LL),
	m_doChronoStat(true) {
   declareProperty("UseDetailChronoStat", m_useDetailChronoStat = false);
   declareProperty("PoolContainerPrefix", m_containerPrefixProp = "CollectionTree");
   declareProperty("TopLevelContainerName", m_containerNameHintProp = "");
   declareProperty("SubLevelBranchName", m_branchNameHintProp = "<type>/<key>");
   declareProperty("PoolAttributes", m_poolAttr);
   declareProperty("InputPoolAttributes", m_inputPoolAttr);
   declareProperty("OutputPoolFileAllocator", m_streamClientFilesProp);
   declareProperty("PrintInputAttrPerEvt", m_inputPoolAttrPerEvent);
   declareProperty("MaxFileSizes", m_maxFileSizes);
   declareProperty("CommitInterval", m_commitInterval = 0);
   declareProperty("SkipFirstChronoCommit", m_skipFirstChronoCommit = false);
   declareProperty("InputStreamingTool", m_inputStreamingTool);
   declareProperty("OutputStreamingTool", m_outputStreamingTool);
   declareProperty("OutputMetadataContainer", m_metadataContainerProp);
}
//______________________________________________________________________________
AthenaPoolCnvSvc::~AthenaPoolCnvSvc() {
}
//__________________________________________________________________________
StatusCode AthenaPoolCnvSvc::decodeOutputSpec(std::string& fileSpec,
		pool::DbType& outputTech) const {
   outputTech = pool::ROOTTREE_StorageType;
   if (fileSpec.find("oracle") == 0 || fileSpec.find("mysql") == 0) {
      outputTech = pool::POOL_RDBMS_StorageType;
   } else if (fileSpec.find("ROOTKEY:") == 0) {
      outputTech = pool::ROOTKEY_StorageType;
      fileSpec.erase(0, 8);
   } else if (fileSpec.find("ROOTTREE:") == 0) {
      outputTech = pool::ROOTTREE_StorageType;
      fileSpec.erase(0, 9);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
void AthenaPoolCnvSvc::extractPoolAttributes(const StringArrayProperty& property,
		std::vector<std::vector<std::string> >* contAttr,
		std::vector<std::vector<std::string> >* dbAttr,
		std::vector<std::vector<std::string> >* domAttr) const {
   std::vector<std::string> opt;
   std::string attributeName, containerName, databaseName, valueString;
   for (std::vector<std::string>::const_iterator iter = property.value().begin(),
           last = property.value().end(); iter != last; iter++) {
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
               if (!m_poolSvc->getAttribute(opt, data, m_dbType.type(), contextId).isSuccess()) {
                  ATH_MSG_DEBUG("getAttribute failed for domain attr " << opt);
                  retError = true;
               }
            }
         } else if (doSet) {
            if (m_poolSvc->setAttribute(opt, data, m_dbType.type(), contextId).isSuccess()) {
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
                  if (!m_poolSvc->getAttribute(opt, data, m_dbType.type(), fileName, cont, contextId).isSuccess()) {
                     ATH_MSG_DEBUG("getAttribute failed for database/container attr " << opt);
                     retError = true;
                  }
               }
            } else if (doSet) {
               if (m_poolSvc->setAttribute(opt, data, m_dbType.type(), fileName, cont, contextId).isSuccess()) {
                  ATH_MSG_DEBUG("setAttribute " << opt << " to " << data << " for db: " << fileName << " and cont: " << cont);
                  if (doClear) {
                     if (file.substr(0, 1) == "*") {
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
         iter++;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
