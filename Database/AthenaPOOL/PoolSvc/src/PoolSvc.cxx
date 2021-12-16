/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/** @file PoolSvc.cxx
 *  @brief This file contains the implementation for the PoolSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "PoolSvc.h"

#include "GaudiKernel/IIoComponentMgr.h"
#include "GaudiKernel/ConcurrencyFlags.h"

#include "PathResolver/PathResolver.h"

#include "CoralKernel/Context.h"

#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"

#include "CollectionBase/CollectionFactory.h"
#include "CollectionBase/CollectionDescription.h"

#include "FileCatalog/IFileCatalog.h"
#include "POOLCore/DbPrint.h"
#include "PersistencySvc/IPersistencySvc.h"
#include "PersistencySvc/ISession.h"
#include "PersistencySvc/IDatabase.h"
#include "PersistencySvc/IContainer.h"
#include "PersistencySvc/ITechnologySpecificAttributes.h"
#include "PersistencySvc/ITokenIterator.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "StorageSvc/DbType.h"

#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/IWebCacheControl.h"
#include "RelationalAccess/IWebCacheInfo.h"
#include "RelationalAccess/ILookupService.h"
#include "RelationalAccess/IDatabaseServiceSet.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"

#include "DBReplicaSvc/IDBReplicaSvc.h"

#include "boost/algorithm/string.hpp" // for starts_with()

#include <cstdlib> 	// for getenv()
#include <cstring> 	// for strcmp()
#include <sys/stat.h> 	// for struct stat
#include <algorithm> 	// for STL find()

//__________________________________________________________________________
StatusCode PoolSvc::initialize() {
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Register this service for 'I/O' events
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_register(this).isSuccess()) {
      ATH_MSG_FATAL("Could not register myself with the IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   // Register input file's names with the I/O manager, unless in SharedWrite mode, set by AthenaPoolCnvSvc
   if (!m_shareCat) {
      bool allGood = true;
      for (auto& catalog : m_readCatalog.value()) {
         if (catalog.compare(0, 16, "xmlcatalog_file:") == 0) {
            const std::string& fileName = catalog.substr(16);
            if (!iomgr->io_register(this, IIoComponentMgr::IoMode::READ, fileName, fileName).isSuccess()) {
               ATH_MSG_FATAL("could not register [" << catalog << "] for input !");
               allGood = false;
            } else {
               ATH_MSG_INFO("io_register[" << this->name() << "](" << catalog << ") [ok]");
            }
         }
      }
      if (m_writeCatalog.value().compare(0, 16, "xmlcatalog_file:") == 0) {
         const std::string& fileName = m_writeCatalog.value().substr(16);
         if (!iomgr->io_register(this, IIoComponentMgr::IoMode::WRITE, fileName, fileName).isSuccess()) {
            ATH_MSG_FATAL("could not register [" << m_writeCatalog.value() << "] for input !");
            allGood = false;
         } else {
            ATH_MSG_INFO("io_register[" << this->name() << "](" << m_writeCatalog.value() << ") [ok]");
         }
      }
      if (!allGood) {
         return(StatusCode::FAILURE);
      }
   }
   m_context = &coral::Context::instance();
   if (m_context == nullptr) {
      ATH_MSG_FATAL("Failed to access CORAL Context");
      return(StatusCode::FAILURE);
   }
   coral::ConnectionService conSvcH;
   coral::IConnectionServiceConfiguration& csConfig = conSvcH.configuration();
   csConfig.setConnectionRetrialPeriod(m_retrialPeriod);
   csConfig.setConnectionRetrialTimeOut(m_retrialTimeOut);
   if (m_connClean) {
      csConfig.enablePoolAutomaticCleanUp();
      csConfig.setConnectionTimeOut(m_timeOut);
   } else {
      csConfig.disablePoolAutomaticCleanUp();
      csConfig.setConnectionTimeOut(0);
   }
   ATH_MSG_INFO("Set connectionsvc retry/timeout/IDLE timeout to "
	   << m_retrialPeriod
	   << "/"
	   << m_retrialTimeOut
	   << "/"
	   << m_timeOut
	   << " seconds with connection cleanup "
	   << (csConfig.isPoolAutomaticCleanUpEnabled() ? "enabled" : "disabled"));
   // set Frontier web cache compression level
   coral::IWebCacheControl& webCache = conSvcH.webCacheControl();
   webCache.setCompressionLevel(m_frontierComp);
   ATH_MSG_INFO("Frontier compression level set to " << webCache.compressionLevel());
   if (m_sortReplicas) {
      // set replica sorter - get service
      IDBReplicaSvc* replicasvc;
      if (Gaudi::svcLocator()->service("DBReplicaSvc", replicasvc).isSuccess()) {
         csConfig.setReplicaSortingAlgorithm(*replicasvc);
         ATH_MSG_INFO("Successfully setup replica sorting algorithm");
      } else {
         ATH_MSG_WARNING("Cannot setup replica sorting algorithm");
      }
   }
   MSG::Level athLvl = msg().level();
   ATH_MSG_DEBUG("OutputLevel is " << (int)athLvl);
   pool::DbPrintLvl::setLevel(athLvl);
   return(setupPersistencySvc());
}

//__________________________________________________________________________
StatusCode PoolSvc::io_reinit() {
   ATH_MSG_INFO("I/O reinitialization...");
   ServiceHandle<IIoComponentMgr> iomgr("IoComponentMgr", name());
   if (!iomgr.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Could not retrieve IoComponentMgr !");
      return(StatusCode::FAILURE);
   }
   if (!iomgr->io_hasitem(this)) {
      ATH_MSG_FATAL("IoComponentMgr does not know about myself !");
      return(StatusCode::FAILURE);
   }
   if (!m_shareCat) {
      std::vector<std::string> readcat = m_readCatalog.value();
      for (std::size_t icat = 0, imax = readcat.size(); icat < imax; icat++) {
         if (readcat[icat].compare(0, 16, "xmlcatalog_file:") == 0) {
            std::string fileName = readcat[icat].substr(16);
            if (iomgr->io_contains(this, fileName)) {
               if (!iomgr->io_retrieve(this, fileName).isSuccess()) {
                  ATH_MSG_FATAL("Could not retrieve new value for [" << fileName << "] !");
                  return(StatusCode::FAILURE);
               }
               readcat[icat] = "xmlcatalog_file:" + fileName;
            }
         }
      }
      // all good... copy over.
      m_readCatalog = readcat;
      if (m_writeCatalog.value().compare(0, 16, "xmlcatalog_file:") == 0) {
         std::string fileName = m_writeCatalog.value().substr(16);
         if (iomgr->io_contains(this, fileName)) {
            if (!iomgr->io_retrieve(this, fileName).isSuccess()) {
               ATH_MSG_FATAL("Could not retrieve new value for [" << fileName << "] !");
               return(StatusCode::FAILURE);
            }
            m_writeCatalog.setValue("xmlcatalog_file:" + fileName);
         }
      }
   }
   return(setupPersistencySvc());
}
//__________________________________________________________________________
StatusCode PoolSvc::setupPersistencySvc() {
   clearState();
   ATH_MSG_INFO("Setting up APR FileCatalog and Streams");
   m_catalog = createCatalog();
   if (m_catalog != nullptr) {
      m_catalog->start();
   } else {
      ATH_MSG_FATAL("Failed to setup POOL File Catalog.");
      return(StatusCode::FAILURE);
   }
   // Setup a persistency services
   m_persistencySvcVec.push_back(pool::IPersistencySvc::create(*m_catalog).release()); // Read Service
   m_pers_mut.push_back(new CallMutex);
   if (!m_persistencySvcVec[IPoolSvc::kInputStream]->session().technologySpecificAttributes(pool::ROOT_StorageType.type()).setAttribute<bool>("ENABLE_THREADSAFETY", true)) {
      ATH_MSG_FATAL("Failed to enable thread safety in ROOT via PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   m_contextMaxFile.insert(std::pair<unsigned int, int>(IPoolSvc::kInputStream, m_dbAgeLimit));
   if (!connect(pool::ITransaction::READ, IPoolSvc::kInputStream).isSuccess()) {
      ATH_MSG_FATAL("Failed to connect Input PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   m_persistencySvcVec.push_back(pool::IPersistencySvc::create(*m_catalog).release()); // Write Service
   m_pers_mut.push_back(new CallMutex);
   pool::DatabaseConnectionPolicy policy;
   policy.setWriteModeForNonExisting(pool::DatabaseConnectionPolicy::CREATE);
   policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::OVERWRITE);
   if (m_fileOpen.value() == "update") {
      policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::UPDATE);
   }
   m_persistencySvcVec[IPoolSvc::kOutputStream]->session().setDefaultConnectionPolicy(policy);
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::start() {
   // Switiching on ROOT implicit multi threading for AthenaMT
   if (m_useROOTIMT && Gaudi::Concurrency::ConcurrencyFlags::numThreads() > 1) {
      if (!m_persistencySvcVec[IPoolSvc::kInputStream]->session().technologySpecificAttributes(pool::ROOT_StorageType.type()).setAttribute<int>("ENABLE_IMPLICITMT", Gaudi::Concurrency::ConcurrencyFlags::numThreads() - 1)) {
         ATH_MSG_FATAL("Failed to enable implicit multithreading in ROOT via PersistencySvc.");
         return(StatusCode::FAILURE);
      }
      ATH_MSG_INFO("Enabled implicit multithreading in ROOT via PersistencySvc to: " << Gaudi::Concurrency::ConcurrencyFlags::numThreads() - 1);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::stop() {
   bool retError = false;
   for (unsigned int contextId = 0, imax = m_persistencySvcVec.size(); contextId < imax; contextId++) {
      if (!disconnect(contextId).isSuccess()) {
         ATH_MSG_FATAL("Cannot disconnect Stream: " << contextId);
         retError = true;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}

//__________________________________________________________________________
void PoolSvc::clearState() {
   // Cleanup persistency service
   for (const auto& persistencySvc : m_persistencySvcVec) {
      delete persistencySvc;
   }
   m_persistencySvcVec.clear();
   for (const auto& persistencyMutex : m_pers_mut) {
      delete persistencyMutex;
   }
   m_mainOutputLabel.clear();
   m_inputContextLabel.clear();
   m_outputContextLabel.clear();
   m_pers_mut.clear();
   if (m_catalog != nullptr) {
      m_catalog->commit();
      delete m_catalog; m_catalog = nullptr;
   }
}
//__________________________________________________________________________
StatusCode PoolSvc::finalize() {
   clearState();
   return(::AthService::finalize());
}
//__________________________________________________________________________
StatusCode PoolSvc::io_finalize() {
   ATH_MSG_INFO("I/O finalization...");
   for (size_t i = 0; i < m_persistencySvcVec.size(); i++) {
      if (m_persistencySvcVec[i]->session().defaultConnectionPolicy().writeModeForNonExisting() != pool::DatabaseConnectionPolicy::RAISE_ERROR &&
	      !disconnect(i).isSuccess()) {
         ATH_MSG_WARNING("Cannot disconnect output Stream " << i);
      }
   }
   clearState();
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode PoolSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (IPoolSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<IPoolSvc*>(this);
   } else {
      // Interface is not directly available: try out a base class
      return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
Token* PoolSvc::registerForWrite(const Placement* placement,
                                 const void* obj,
                                 const RootType& classDesc) {
   unsigned int contextId = IPoolSvc::kOutputStream;
   const std::string& auxString = placement->auxString();
   if (!auxString.empty()) {
      if (auxString.compare(0, 6, "[CTXT=") == 0) {
         ::sscanf(auxString.c_str(), "[CTXT=%08X]", &contextId);
      } else if (auxString.compare(0, 8, "[CLABEL=") == 0) {
         contextId = this->getOutputContext(auxString);
      }
      if (contextId >= m_persistencySvcVec.size()) {
         ATH_MSG_WARNING("registerForWrite: Using default output Stream instead of id = " << contextId);
         contextId = IPoolSvc::kOutputStream;
      }
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   Token* token = m_persistencySvcVec[contextId]->registerForWrite(*placement, obj, classDesc);
   if (token == nullptr) {
      ATH_MSG_WARNING("Cannot write object: " << placement->containerName());
   }
   return(token);
}
//__________________________________________________________________________
void PoolSvc::setObjPtr(void*& obj, const Token* token) {
   unsigned int contextId = IPoolSvc::kInputStream;
   const std::string& auxString = token->auxString();
   if (!auxString.empty()) {
      if (auxString.compare(0, 6, "[CTXT=") == 0) {
         ::sscanf(auxString.c_str(), "[CTXT=%08X]", &contextId);
      } else if (auxString.compare(0, 8, "[CLABEL=") == 0) {
         contextId = this->getInputContext(auxString);
      }
      if (contextId >= m_persistencySvcVec.size()) {
         ATH_MSG_WARNING("setObjPtr: Using default input Stream instead of id = " << contextId);
         contextId = IPoolSvc::kInputStream;
      }
   }
   ATH_MSG_VERBOSE("setObjPtr: token=" << token->toString() << ", auxString=" << auxString << ", contextID=" << contextId);
   // Get Context ID/label from Token
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   obj = m_persistencySvcVec[contextId]->readObject(*token, obj);
   std::map<unsigned int, unsigned int>::const_iterator maxFileIter = m_contextMaxFile.find(contextId);
   if (maxFileIter != m_contextMaxFile.end() && maxFileIter->second > 0) {
      m_guidLists[contextId].remove(token->dbID());
      m_guidLists[contextId].push_back(token->dbID());
      while (m_guidLists[contextId].size() > maxFileIter->second) {
         this->disconnectDb("FID:" + m_guidLists[contextId].begin()->toString(), contextId).ignore();
      }
   }
}
//__________________________________________________________________________
unsigned int PoolSvc::getOutputContext(const std::string& label) {
   std::lock_guard<CallMutex> lock(m_pool_mut);
   if (m_mainOutputLabel.empty()) {
      m_mainOutputLabel = label;
      m_outputContextLabel.insert(std::pair<std::string, unsigned int>(label, IPoolSvc::kOutputStream));
   }
   if (label == m_mainOutputLabel || label.empty()) {
      return(IPoolSvc::kOutputStream);
   }
   std::map<std::string, unsigned int>::const_iterator contextIter = m_outputContextLabel.find(label);
   if (contextIter != m_outputContextLabel.end()) {
      return(contextIter->second);
   }
   const unsigned int id = m_persistencySvcVec.size();
   m_persistencySvcVec.push_back(pool::IPersistencySvc::create(*m_catalog).release());
   m_pers_mut.push_back(new CallMutex);
   pool::DatabaseConnectionPolicy policy;
   policy.setWriteModeForNonExisting(pool::DatabaseConnectionPolicy::CREATE);
   policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::OVERWRITE);
   if (m_fileOpen.value() == "update") {
      policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::UPDATE);
   }
   m_persistencySvcVec[id]->session().setDefaultConnectionPolicy(policy);
   m_outputContextLabel.insert(std::pair<std::string, unsigned int>(label, id));
   return(id);
}
//__________________________________________________________________________
unsigned int PoolSvc::getInputContext(const std::string& label, unsigned int maxFile, const std::string& fileID) {
   std::lock_guard<CallMutex> lock(m_pool_mut);
   if (!label.empty()) {
      std::map<std::string, unsigned int>::const_iterator contextIter = m_inputContextLabel.find(label);
      if (contextIter != m_inputContextLabel.end()) {
         if (maxFile > 0) {
            m_contextMaxFile[contextIter->second] = maxFile;
         }
         return(contextIter->second);
      }
   }
   const unsigned int id = m_persistencySvcVec.size();
   m_persistencySvcVec.push_back( pool::IPersistencySvc::create(*m_catalog).release() );
   m_pers_mut.push_back(new CallMutex);
   if (!connect(pool::ITransaction::READ, id).isSuccess()) {
      ATH_MSG_WARNING("Failed to connect Input PersistencySvc: " << id);
      return(IPoolSvc::kInputStream);
   }
   if (!label.empty()) {
      m_inputContextLabel.insert(std::pair<std::string, unsigned int>(label, id));
   }
   m_contextMaxFile.insert(std::pair<unsigned int, int>(id, maxFile));
   if (!fileID.empty()) {
      if (!this->setAttribute("TREE_CACHE", "0", pool::ROOT_StorageType.type(), fileID, "CollectionTree", id).isSuccess()) {
         ATH_MSG_WARNING("Failed to switch off TTreeCache.");
      }
   }
   return(id);
}
//__________________________________________________________________________
const std::map<std::string, unsigned int>& PoolSvc::getInputContextMap() const {
   return(m_inputContextLabel);
}
//__________________________________________________________________________
const coral::Context* PoolSvc::context() const {
   return(m_context);
}
//__________________________________________________________________________
void PoolSvc::loadComponent(const std::string& compName) const {
   m_context->loadComponent(compName);
}
//__________________________________________________________________________
void PoolSvc::setShareMode(bool shareCat) {
   m_shareCat = shareCat;
}
//__________________________________________________________________________
const pool::IFileCatalog* PoolSvc::catalog() const {
   return(m_catalog);
}
//__________________________________________________________________________
void PoolSvc::lookupBestPfn(const std::string& token, std::string& pfn, std::string& type) const {
   std::string dbID;
   if (token.compare(0, 4, "PFN:") == 0) {
      m_catalog->lookupFileByPFN(token.substr(4), dbID, type); // PFN -> FID
   } else if (token.compare(0, 4, "LFN:") == 0) {
      m_catalog->lookupFileByLFN(token.substr(4), dbID); // LFN -> FID
   } else if (token.compare(0, 4, "FID:") == 0) {
      dbID = token.substr(4);
   } else if (token.size() > Guid::null().toString().size()) { // full token
      Token tok;
      tok.fromString(token);
      dbID = tok.dbID().toString();
   } else { // guid only
      dbID = token;
   }
   m_catalog->getFirstPFN(dbID, pfn, type); // FID -> best PFN
}
//__________________________________________________________________________
void PoolSvc::renamePfn(const std::string& pf, const std::string& newpf) const {
   std::string dbID, type;
    m_catalog->lookupFileByPFN(pf, dbID, type);
   if (dbID.empty()) {
      ATH_MSG_WARNING("Failed to lookup: " << pf << " in FileCatalog");
      return;
   }
   m_catalog->lookupFileByPFN(newpf, dbID, type);
   if (!dbID.empty()) {
      ATH_MSG_INFO("Found: " << newpf << " in FileCatalog");
      return;
   }
   m_catalog->renamePFN(pf, newpf);
}
//__________________________________________________________________________
pool::ICollection* PoolSvc::createCollection ATLAS_NOT_THREAD_SAFE
               (const std::string& collectionType,
		const std::string& connection,
		const std::string& collectionName,
		unsigned int contextId) const {
   ATH_MSG_DEBUG("createCollection() type="<< collectionType << ", connection=" << connection
                 << ", name=" << collectionName << ", contextID=" << contextId);
   std::string collection(collectionName);
   if (collectionType == "RootCollection") {
      if (collectionName.find("PFN:") == std::string::npos
	      && collectionName.find("LFN:") == std::string::npos
	      && collectionName.find("FID:") == std::string::npos) {
	 collection = "PFN:" + collectionName;
      }
   }
   if (contextId >= m_persistencySvcVec.size()) {
      ATH_MSG_WARNING("createCollection: Using default input Stream instead of id = " << contextId);
      contextId = IPoolSvc::kInputStream;
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   // Check POOL FileCatalog entry.
   bool insertFile = false;
   if (connection.compare(0, 4, "PFN:") == 0) {
      std::string fid, fileType;
      m_catalog->lookupFileByPFN(connection.substr(4), fid, fileType);
      if (fid.empty()) { // No entry in file catalog
         insertFile = true;
         ATH_MSG_INFO("File is not in Catalog! Attempt to open it anyway.");
      }
   }
   // Check whether Collection Container exists.
   if (collectionType == "ImplicitCollection") {
      std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, connection);
      if (dbH == nullptr) {
         ATH_MSG_INFO("Failed to get Session/DatabaseHandle to create POOL collection.");
         return(nullptr);
      }
      try {
         if (dbH->openMode() == pool::IDatabase::CLOSED) {
            dbH->connectForRead();
         }
         std::map<unsigned int, unsigned int>::const_iterator maxFileIter = m_contextMaxFile.find(contextId);
         if (maxFileIter != m_contextMaxFile.end() && maxFileIter->second > 0 && !dbH->fid().empty()) {
            const Guid guid(dbH->fid());
            m_guidLists[contextId].remove(guid);
            m_guidLists[contextId].push_back(guid);
            while (m_guidLists[contextId].size() > maxFileIter->second + 1) {
               this->disconnectDb("FID:" + m_guidLists[contextId].begin()->toString(), contextId).ignore();
            }
         }
         std::unique_ptr<pool::IContainer> contH = getContainerHandle(dbH.get(), collection);
         if (contH == nullptr) {
            ATH_MSG_INFO("Failed to find container " << collection << " to create POOL collection.");
            if (insertFile && m_attemptCatalogPatch.value()) {
               dbH->setTechnology(pool::ROOT_StorageType.type());
               std::string fid = dbH->fid();
               m_catalog->registerPFN(connection.substr(4), "ROOT_All", fid);
            }
            return(nullptr); // no events
         }
      } catch(std::exception& e) {
         ATH_MSG_INFO("Failed to open container to check POOL collection - trying.");
      }
   }
   pool::CollectionFactory* collFac = pool::CollectionFactory::get();
   pool::CollectionDescription collDes(collection, collectionType, collectionType == "ImplicitCollection" ? connection : "");
   pool::ICollection* collPtr = nullptr;
   if (collectionType == "RootCollection" &&
	   m_persistencySvcVec[contextId]->session().defaultConnectionPolicy().writeModeForNonExisting() != pool::DatabaseConnectionPolicy::RAISE_ERROR) {
      ATH_MSG_INFO("Writing ExplicitROOT Collection - do not pass session pointer");
      collPtr = collFac->create(collDes,  pool::ICollection::READ);
   } else {
      try {
         collPtr = collFac->create(collDes, pool::ICollection::READ, &m_persistencySvcVec[contextId]->session());
      } catch (std::exception &e) {
         if (insertFile) {
            std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, connection);
            if (dbH != nullptr) {
               if (!dbH->fid().empty()) {
                  return(nullptr); // no events
               }
            }
         }
         throw; // bad file, rethrow
      }
   }
   if (insertFile && m_attemptCatalogPatch.value()) {
      std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, connection);
      if (dbH == nullptr) {
         ATH_MSG_INFO("Failed to create FileCatalog entry.");
      } else if (dbH->fid().empty()) {
         ATH_MSG_INFO("Cannot retrieve the FID of an existing POOL database: '"
                      << connection << "' - FileCatalog will NOT be updated.");
      } else {
         dbH->setTechnology(pool::ROOT_StorageType.type());
         std::string fid = dbH->fid();
         m_catalog->registerPFN(connection.substr(4), "ROOT_All", fid);
      }
   }
   // For multithreaded processing (with multiple events in flight),
   // increase virtual tree size to accomodate back reads
   if (m_useROOTMaxTree && Gaudi::Concurrency::ConcurrencyFlags::numConcurrentEvents() > 1) {
      if (!this->setAttribute("TREE_MAX_VIRTUAL_SIZE", "-1", pool::ROOT_StorageType.type(), connection.substr(4), "CollectionTree", IPoolSvc::kInputStream).isSuccess()) {
         ATH_MSG_WARNING("Failed to increase maximum virtual TTree size.");
      }
   }

   return(collPtr);
}
//__________________________________________________________________________
Token* PoolSvc::getToken(const std::string& connection,
	const std::string& collection,
	const unsigned long ientry) const {
   std::lock_guard<CallMutex> lock(*m_pers_mut[IPoolSvc::kInputStream]);
   std::unique_ptr<pool::IDatabase> dbH = getDbHandle(IPoolSvc::kInputStream, connection);
   if (dbH == nullptr) {
      return(nullptr);
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      dbH->connectForRead();
   }
   std::unique_ptr<pool::IContainer> contH = getContainerHandle(dbH.get(), collection);
   if (contH == nullptr) {
      return(nullptr);
   }
   pool::ITokenIterator* tokenIter = contH->tokens("");
   Token* thisToken = tokenIter->next();
   for (unsigned long ipos = 0; ipos < ientry; ipos++) {
      delete thisToken; thisToken = tokenIter->next();
   }
   delete tokenIter; tokenIter = nullptr;
   return(thisToken);
}
//__________________________________________________________________________
StatusCode PoolSvc::connect(pool::ITransaction::Type type, unsigned int contextId) {
   if (type != pool::ITransaction::READ) {
      if (contextId >= m_persistencySvcVec.size()) {
         ATH_MSG_WARNING("connect: Using default output Stream instead of id = " << contextId);
         contextId = IPoolSvc::kOutputStream;
      }
   } else {
      if (contextId > m_persistencySvcVec.size()) {
         ATH_MSG_WARNING("connect: Using default input Stream instead of id = " << contextId);
         contextId = IPoolSvc::kInputStream;
      } else if (contextId == m_persistencySvcVec.size()) {
         ATH_MSG_INFO("Connecting to InputStream for: " << contextId);
         contextId = this->getInputContext("");
      }
   }
   if (contextId >= m_persistencySvcVec.size()) {
      return(StatusCode::FAILURE);
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   pool::IPersistencySvc* persSvc = m_persistencySvcVec[contextId];
   // Connect to a logical database using the pre-defined technology and dbID
   if (persSvc->session().transaction().isActive()) {
      return(StatusCode::SUCCESS);
   }
   if (!persSvc->session().transaction().start(type)) {
      ATH_MSG_ERROR("connect failed persSvc = " << persSvc << " type = " << type);
      return(StatusCode::FAILURE);
   }

   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::commit(unsigned int contextId) const {
   if (contextId >= m_persistencySvcVec.size()) {
      return(StatusCode::FAILURE);
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   pool::IPersistencySvc* persSvc = m_persistencySvcVec[contextId];
   if (persSvc != nullptr && persSvc->session().transaction().isActive()) {
      if (!persSvc->session().transaction().commit()) {
         ATH_MSG_ERROR("POOL commit failed " << persSvc);
         return(StatusCode::FAILURE);
      }
      if (persSvc->session().transaction().type() == pool::ITransaction::READ) {
         persSvc->session().disconnectAll();
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::commitAndHold(unsigned int contextId) const {
   if (contextId >= m_persistencySvcVec.size()) {
      return(StatusCode::FAILURE);
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   pool::IPersistencySvc* persSvc = m_persistencySvcVec[contextId];
   if (persSvc != nullptr && persSvc->session().transaction().isActive()) {
      if (!persSvc->session().transaction().commitAndHold()) {
         ATH_MSG_ERROR("POOL commitAndHold failed " << persSvc);
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::disconnect(unsigned int contextId) const {
   ATH_MSG_DEBUG("Disconnect request for contextId=" << contextId);
   if (contextId >= m_persistencySvcVec.size()) {
      return(StatusCode::SUCCESS);
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   pool::IPersistencySvc* persSvc = m_persistencySvcVec[contextId];
   if (persSvc != nullptr && persSvc->session().transaction().isActive()) {
      if (!commit(contextId).isSuccess()) {
         ATH_MSG_ERROR("disconnect failed to commit " << persSvc);
         return(StatusCode::FAILURE);
      }
      persSvc->session().disconnectAll();
      ATH_MSG_DEBUG("Disconnected PersistencySvc session");
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::disconnectDb(const std::string& connection, unsigned int contextId) const {
   if (contextId >= m_persistencySvcVec.size()) {
      return(StatusCode::SUCCESS);
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, connection);
   if (dbH == nullptr) {
      ATH_MSG_ERROR("Failed to get Session/DatabaseHandle.");
      return(StatusCode::FAILURE);
   }
   std::map<unsigned int, unsigned int>::const_iterator maxFileIter = m_contextMaxFile.find(contextId);
   if (maxFileIter != m_contextMaxFile.end() && maxFileIter->second > 0) {
      m_guidLists[contextId].remove(Guid(dbH->fid()));
   }
   dbH->disconnect();
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
long long int PoolSvc::getFileSize(const std::string& dbName, long tech, unsigned int contextId) const {
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, dbName);
   if (dbH == nullptr) {
      ATH_MSG_DEBUG("getFileSize: Failed to get Session/DatabaseHandle to get POOL FileSize property.");
      return 0; // failure
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      if (m_persistencySvcVec[contextId]->session().defaultConnectionPolicy().writeModeForNonExisting() != pool::DatabaseConnectionPolicy::RAISE_ERROR) {
         dbH->setTechnology(tech);
         dbH->connectForWrite();
      } else {
         dbH->connectForRead();
      }
   }
   return(dbH->technologySpecificAttributes().attribute<long long int>("FILE_SIZE"));
}
//_______________________________________________________________________
StatusCode PoolSvc::getAttribute(const std::string& optName,
		std::string& data,
		long tech,
		unsigned int contextId) const {
   if (contextId >= m_persistencySvcVec.size()) {
      ATH_MSG_WARNING("getAttribute: Using default input Stream instead of id = " << contextId);
      contextId = IPoolSvc::kInputStream;
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   pool::ISession& sesH = m_persistencySvcVec[contextId]->session();
   std::ostringstream oss;
   if (data == "DbLonglong") {
      oss << std::dec << sesH.technologySpecificAttributes(tech).attribute<long long int>(optName);
   } else if (data == "double") {
      oss << std::dec << sesH.technologySpecificAttributes(tech).attribute<double>(optName);
   } else {
      oss << std::dec << sesH.technologySpecificAttributes(tech).attribute<int>(optName);
   }
   data = oss.str();
   ATH_MSG_INFO("Domain attribute [" << optName << "]" << ": " << data);
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode PoolSvc::getAttribute(const std::string& optName,
		std::string& data,
		long tech,
		const std::string& dbName,
		const std::string& contName,
		unsigned int contextId) const {
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, dbName);
   if (dbH == nullptr) {
      ATH_MSG_DEBUG("getAttribute: Failed to get Session/DatabaseHandle to get POOL property.");
      return(StatusCode::FAILURE);
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      if (m_persistencySvcVec[contextId]->session().defaultConnectionPolicy().writeModeForNonExisting() != pool::DatabaseConnectionPolicy::RAISE_ERROR) {
         dbH->setTechnology(tech);
         dbH->connectForWrite();
      } else {
         dbH->connectForRead();
      }
   }
   std::ostringstream oss;
   if (contName.empty()) {
      if (data == "DbLonglong") {
         oss << std::dec << dbH->technologySpecificAttributes().attribute<long long int>(optName);
      } else if (data == "double") {
         oss << std::dec << dbH->technologySpecificAttributes().attribute<double>(optName);
      } else if (data == "string") {
         oss << dbH->technologySpecificAttributes().attribute<char*>(optName);
      } else {
         oss << std::dec << dbH->technologySpecificAttributes().attribute<int>(optName);
      }
      ATH_MSG_INFO("Database (" << dbH->pfn() << ") attribute [" << optName << "]" << ": " << oss.str());
   } else {
      std::unique_ptr<pool::IContainer> contH = getContainerHandle(dbH.get(), contName);
      if (contH == nullptr) {
         ATH_MSG_DEBUG("Failed to get ContainerHandle to get POOL property.");
         return(StatusCode::FAILURE);
      }
      if (data == "DbLonglong") {
         oss << std::dec << contH->technologySpecificAttributes().attribute<long long int>(optName);
      } else if (data == "double") {
         oss << std::dec << contH->technologySpecificAttributes().attribute<double>(optName);
      } else {
         oss << std::dec << contH->technologySpecificAttributes().attribute<int>(optName);
      }
      ATH_MSG_INFO("Container attribute [" << contName << "." << optName << "]: " << oss.str());
   }
   data = oss.str();
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode PoolSvc::setAttribute(const std::string& optName,
		const std::string& data,
		long tech,
		unsigned int contextId) const {
   if (contextId >= m_persistencySvcVec.size()) {
      ATH_MSG_WARNING("setAttribute: Using default output Stream instead of id = " << contextId);
      contextId = IPoolSvc::kOutputStream;
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   pool::ISession& sesH = m_persistencySvcVec[contextId]->session();
   if (data[data.size() - 1] == 'L') {
      if (!sesH.technologySpecificAttributes(tech).setAttribute<long long int>(optName, atoll(data.c_str()))) {
         ATH_MSG_DEBUG("Failed to set POOL property, " << optName << " to " << data);
         return(StatusCode::FAILURE);
      }
   } else {
      if (!sesH.technologySpecificAttributes(tech).setAttribute<int>(optName, atoi(data.c_str()))) {
         ATH_MSG_DEBUG("Failed to set POOL property, " << optName << " to " << data);
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode PoolSvc::setAttribute(const std::string& optName,
		const std::string& data,
		long tech,
		const std::string& dbName,
		const std::string& contName,
		unsigned int contextId) const {
   if (contextId >= m_persistencySvcVec.size()) {
      ATH_MSG_WARNING("setAttribute: Using default output Stream instead of id = " << contextId);
      contextId = IPoolSvc::kOutputStream;
   }
   std::lock_guard<CallMutex> lock(*m_pers_mut[contextId]);
   std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, dbName);
   if (dbH == nullptr) {
      ATH_MSG_DEBUG("Failed to get Session/DatabaseHandle to set POOL property.");
      return(StatusCode::FAILURE);
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      if (m_persistencySvcVec[contextId]->session().defaultConnectionPolicy().writeModeForNonExisting() != pool::DatabaseConnectionPolicy::RAISE_ERROR) {
         dbH->setTechnology(tech);
         dbH->connectForWrite();
      } else {
         dbH->connectForRead();
      }
   }
   bool retError = false;
   std::string objName;
   bool hasTTreeName = (contName.length() > 6 && contName.compare(0, 6, "TTree=") == 0);
   if (contName.empty() || hasTTreeName || m_persistencySvcVec[contextId]->session().defaultConnectionPolicy().writeModeForNonExisting() == pool::DatabaseConnectionPolicy::RAISE_ERROR) {
      objName = hasTTreeName ? contName.substr(6) : contName;
      if (!objName.empty()) {
         bool found = false;
         const std::vector<std::string>& containers = dbH->containers();
         for (std::vector<std::string>::const_iterator iter = containers.begin(), last = containers.end(); iter != last; ++iter) {
            if (*iter == objName || boost::starts_with(*iter, objName + "(") || boost::starts_with(*iter, objName + "_")) {
               found = true;
               break;
            }
         }
         if (!found) {
            ATH_MSG_DEBUG("Failed to find TTree: " << objName << " to set POOL property.");
            return(StatusCode::FAILURE);
         }
      }
      if (data[data.size() - 1] == 'L') {
         retError = dbH->technologySpecificAttributes().setAttribute<long long int>(optName, atoll(data.c_str()), objName);
      } else {
         retError = dbH->technologySpecificAttributes().setAttribute<int>(optName, atoi(data.c_str()), objName);
      }
      if (!retError) {
         ATH_MSG_DEBUG("Failed to set POOL property, " << optName << " to " << data);
         return(StatusCode::FAILURE);
      }
   } else {
      std::unique_ptr<pool::IContainer> contH = getContainerHandle(dbH.get(), contName);
      if (contH == nullptr) {
         ATH_MSG_DEBUG("Failed to get ContainerHandle to set POOL property.");
         return(StatusCode::FAILURE);
      }
      if (auto p = contName.find('('); p != std::string::npos) {
         objName = contName.substr(p + 1); // Get BranchName between parenthesis
         objName = objName.substr(0, objName.find(')'));
      } else if (auto p = contName.find("::"); p != std::string::npos) {
         objName = contName.substr(p + 2); // Split off Tree name
      } else if (auto p = contName.find('_'); p != std::string::npos) {
         objName = contName.substr(p + 1); // Split off "POOLContainer"
         objName = objName.substr(0, objName.find('/')); // Split off key
      }
      std::string::size_type off = 0;
      while ((off = objName.find_first_of("<>/")) != std::string::npos) {
         objName[off] = '_'; // Replace special chars (e.g. templates)
      }
      if (data[data.size() - 1] == 'L') {
         retError = contH->technologySpecificAttributes().setAttribute<long long int>(optName, atoll(data.c_str()), objName);
      } else {
         retError = contH->technologySpecificAttributes().setAttribute<int>(optName, atoi(data.c_str()), objName);
      }
      if (!retError) {
         ATH_MSG_DEBUG("Failed to set POOL container property, " << optName << " for " << contName << " : " << objName << " to " << data);
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::setFrontierCache(const std::string& conn) const {
   std::lock_guard<CallMutex> lock(m_pool_mut);
   ATH_MSG_VERBOSE("setFrontierCache called for connection:" << conn);
   // setup the Frontier cache information for the given logical or physical connection string
   // first determine if the connection is logical (no ':')
   std::vector<std::string> physcons;
   if (conn.find(':') == std::string::npos) {
      // if logical, have to lookup list of physical replicas, and consider each
      // need the CORAL ILookupSvc interface which must be loaded if needed
      const std::string lookSvcStr("CORAL/Services/XMLLookupService");
      coral::IHandle<coral::ILookupService> lookSvcH = m_context->query<coral::ILookupService>();
      if (!lookSvcH.isValid()) {
         m_context->loadComponent(lookSvcStr);
         lookSvcH = m_context->query<coral::ILookupService>();
      }
      if (!lookSvcH.isValid()) {
         ATH_MSG_ERROR("Cannot locate " << lookSvcStr);
         return(StatusCode::FAILURE);
      }
      coral::IDatabaseServiceSet* dbset = lookSvcH->lookup(conn, coral::ReadOnly);
      if (dbset != nullptr) {
         for (int irep = 0, nrep = dbset->numberOfReplicas(); irep < nrep; ++irep) {
	    const std::string pcon = dbset->replica(irep).connectionString();
	    if (pcon.compare(0, 9, "frontier:") == 0) {
               physcons.push_back(pcon);
            }
         }
         delete dbset; dbset = nullptr;
      } else {
         ATH_MSG_DEBUG("setFrontierCache: Could not find any replicas for " << conn);
      }
   } else if (conn.compare(0, 9, "frontier:") == 0) {
      physcons.push_back(conn);
   }
   // check if any replicas will try and use frontier
   if (physcons.size() == 0) {
      return(StatusCode::SUCCESS);
   }
   coral::ConnectionService conSvcH;
   // for each frontier replica, define the web cache info
   // get the WebCacheControl interface via ConnectionSvc
   // note ConnectionSvc should already be loaded by initialize
   coral::IWebCacheControl& webCache = conSvcH.webCacheControl();
   for (std::vector<std::string>::const_iterator iter = physcons.begin(), last = physcons.end();
		   iter != last; ++iter) {
      if (find(m_frontierRefresh.value().begin(), m_frontierRefresh.value().end(), *iter)
		      == m_frontierRefresh.value().end()
	      && find(m_frontierRefresh.value().begin(), m_frontierRefresh.value().end(), conn)
		      == m_frontierRefresh.value().end()) {
         // set that a table DUMMYTABLE should be refreshed - indicates that everything
         // else in the schema should not be
         webCache.refreshTable(*iter, "DUMMYTABLE");
      } else {
         // set the schema to be refreshed
         webCache.refreshSchemaInfo(*iter);
      }
      ATH_MSG_DEBUG("Cache flag for connection " << *iter << " set to " << webCache.webCacheInfo(*iter).isSchemaInfoCached());
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
pool::IFileCatalog* PoolSvc::createCatalog() {
   pool::IFileCatalog* ctlg = new pool::IFileCatalog;
   ctlg->removeCatalog("*");
   for (auto& catalog : m_readCatalog.value()) {
      ATH_MSG_DEBUG("POOL ReadCatalog is " << catalog);
      if (catalog.compare(0, 8,"apcfile:") == 0 || catalog.compare(0, 7, "prfile:") == 0) {
         std::string::size_type cpos = catalog.find(':');
         // check for file accessed via ATLAS_POOLCOND_PATH
         std::string file = poolCondPath(catalog.substr(cpos + 1));
         if (!file.empty()) {
            ATH_MSG_INFO("Resolved path (via ATLAS_POOLCOND_PATH) is " << file);
            ctlg->addReadCatalog("file:" + file);
         } else {
            // As backup, check for file accessed via PathResolver
            file = PathResolver::find_file(catalog.substr(cpos + 1), "DATAPATH");
            if (!file.empty()) {
               ATH_MSG_INFO("Resolved path (via DATAPATH) is " << file);
               ctlg->addReadCatalog("file:" + file);
            } else {
               ATH_MSG_INFO("Unable find catalog "
	               << catalog
	               << " in $ATLAS_POOLCOND_PATH and $DATAPATH");
            }
         }
      } else {
         ctlg->addReadCatalog(catalog);
      }
   }
   try {
      ATH_MSG_INFO("POOL WriteCatalog is " << m_writeCatalog.value());
      ctlg->setWriteCatalog(m_writeCatalog.value());
      ctlg->connect();
   } catch(std::exception& e) {
      ATH_MSG_ERROR("setWriteCatalog - caught exception: " << e.what());
      return(nullptr); // This catalog is not setup properly!
   }
   return(ctlg);
}

//__________________________________________________________________________
PoolSvc::~PoolSvc() {
}
//__________________________________________________________________________
std::unique_ptr<pool::IDatabase> PoolSvc::getDbHandle(unsigned int contextId, const std::string& dbName) const {
   pool::IDatabase* dbH = nullptr;
   if (contextId >= m_persistencySvcVec.size()) {
      ATH_MSG_WARNING("getDbHandle: Using default input Stream instead of id = " << contextId);
      contextId = IPoolSvc::kInputStream;
   }
   pool::ISession& sesH = m_persistencySvcVec[contextId]->session();
   if (!sesH.transaction().isActive()) {
      pool::ITransaction::Type transMode = pool::ITransaction::READ;
      if (m_persistencySvcVec[contextId]->session().defaultConnectionPolicy().writeModeForNonExisting() != pool::DatabaseConnectionPolicy::RAISE_ERROR) {
         transMode = pool::ITransaction::UPDATE;
      }
      ATH_MSG_DEBUG("Start transaction, type = " << transMode);
      if (!sesH.transaction().start(transMode)) {
         ATH_MSG_WARNING("Failed to start transaction, type = " << transMode);
         return(nullptr);
      }
   }
   if (dbName.compare(0, 4,"PFN:") == 0) {
      dbH = sesH.databaseHandle(dbName.substr(4), pool::DatabaseSpecification::PFN);
   } else if (dbName.compare(0, 4, "LFN:") == 0) {
      dbH = sesH.databaseHandle(dbName.substr(4), pool::DatabaseSpecification::LFN);
   } else if (dbName.compare(0, 4,"FID:") == 0) {
      dbH = sesH.databaseHandle(dbName.substr(4), pool::DatabaseSpecification::FID);
   } else {
      dbH = sesH.databaseHandle(dbName, pool::DatabaseSpecification::PFN);
   }
   return(std::unique_ptr<pool::IDatabase>(dbH));
}
//__________________________________________________________________________
std::unique_ptr<pool::IContainer> PoolSvc::getContainerHandle(pool::IDatabase* dbH, const std::string& contName) const {
   pool::IContainer* contH = nullptr;
   if (dbH == nullptr) {
      ATH_MSG_DEBUG("No DatabaseHandle to get Container.");
      return(nullptr);
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      dbH->connectForRead();
   }
   if (contName.find("DataHeader") != std::string::npos) {
      contH = dbH->containerHandle(contName.substr(0, contName.find("_p")));
   } else {
      contH = dbH->containerHandle(contName);
   }
   return(std::unique_ptr<pool::IContainer>(contH));
}
//__________________________________________________________________________
std::string PoolSvc::poolCondPath(const std::string& leaf) {
   // look for files at $ATLAS_POOLCOND_PATH/poolcond/<leaf>
   // return full filename if exists, or empty string if not
   std::string respath;
   const char* cpath = getenv("ATLAS_POOLCOND_PATH");
   if (cpath && strcmp(cpath, "") != 0) {
      std::string testpath = cpath;
      testpath += '/';
      testpath += leaf;
      struct stat stFileInfo;
      // try to retrieve file attribute - success indicates file exists
      if (stat(testpath.c_str(), &stFileInfo) == 0) {
         respath = std::move(testpath);
      }
   }
   return(respath);
}
