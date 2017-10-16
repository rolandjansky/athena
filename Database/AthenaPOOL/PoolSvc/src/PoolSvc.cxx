/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file PoolSvc.cxx
 *  @brief This file contains the implementation for the PoolSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "PoolSvc.h"

#include "GaudiKernel/IIoComponentMgr.h"

#include "PathResolver/PathResolver.h"

#include "CoralKernel/Context.h"

#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"

#include "CollectionBase/CollectionFactory.h"
#include "CollectionBase/CollectionDescription.h"

#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/IFCAction.h"

#include "PersistencySvc/IPersistencySvcFactory.h"
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
   // Register input file's names with the I/O manager
   const std::vector<std::string>& readcat = m_readCatalog.value();
   bool allGood = true;
   std::string fileName;
   for (std::size_t icat = 0, imax = readcat.size(); icat < imax; icat++) {
      if (readcat[icat].substr(0, 16) == "xmlcatalog_file:") {
         fileName = readcat[icat].substr(16);
         if (!iomgr->io_register(this, IIoComponentMgr::IoMode::READ, fileName, fileName).isSuccess()) {
            ATH_MSG_FATAL("could not register [" << readcat[icat] << "] for input !");
            allGood = false;
         } else {
            ATH_MSG_INFO("io_register[" << this->name() << "](" << readcat[icat] << ") [ok]");
         }
      }
   }
   if (m_writeCatalog.value().substr(0, 16) == "xmlcatalog_file:") {
      fileName = m_writeCatalog.value().substr(16);
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
   ATH_MSG_DEBUG("OutputLevel is " << athLvl);
   pool::DbPrintLvl::setLevel( athLvl );

   return setupPersistencySvc();
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
   std::vector<std::string> readcat = m_readCatalog.value();
   for (std::size_t icat = 0, imax = readcat.size(); icat < imax; icat++) {
      if (readcat[icat].substr(0, 16) == "xmlcatalog_file:") {
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
   if (m_writeCatalog.value().substr(0, 16) == "xmlcatalog_file:") {
      std::string fileName = m_writeCatalog.value().substr(16);
      if (iomgr->io_contains(this, fileName)) {
         if (!iomgr->io_retrieve(this, fileName).isSuccess()) {
            ATH_MSG_FATAL("Could not retrieve new value for [" << fileName << "] !");
            return(StatusCode::FAILURE);
         }
         m_writeCatalog.setValue("xmlcatalog_file:" + fileName);
      }
   }
   return setupPersistencySvc();
}
//__________________________________________________________________________
StatusCode PoolSvc::setupPersistencySvc() {
   ATH_MSG_INFO("Setting up APR FileCatalog and Streams");
   if (m_catalog != nullptr) {
      m_catalog->commit();
      delete m_catalog; m_catalog = nullptr;
   }
   m_catalog = createCatalog();
   if (m_catalog != nullptr) {
      m_catalog->start();
   } else {
      ATH_MSG_FATAL("Failed to setup POOL File Catalog.");
      return(StatusCode::FAILURE);
   }
   // Setup a persistency service
   for (std::vector<pool::IPersistencySvc*>::const_iterator iter = m_persistencySvcVec.begin(),
		   last = m_persistencySvcVec.end(); iter != last; iter++) {
      delete *iter;
   }
   m_persistencySvcVec.clear();
   for (std::vector<CallMutex*>::const_iterator iter = m_pers_mut.begin(),
		   last = m_pers_mut.end(); iter != last; iter++) {
      delete *iter;
   }
   m_pers_mut.clear();
   pool::IPersistencySvcFactory* psfactory = pool::IPersistencySvcFactory::get();
   if (psfactory == nullptr) {
      ATH_MSG_FATAL("Failed to create PersistencySvcFactory.");
      return(StatusCode::FAILURE);
   }
   m_persistencySvcVec.push_back(psfactory->create("PersistencySvc", *m_catalog)); // Read Service
   if (m_persistencySvcVec[IPoolSvc::kInputStream] == nullptr) {
      ATH_MSG_FATAL("Failed to create Input PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   m_pers_mut.push_back(new CallMutex);
   if (!m_persistencySvcVec[IPoolSvc::kInputStream]->session().technologySpecificAttributes(pool::ROOT_StorageType.type()).setAttribute<bool>("MultiThreaded", true)) {
      ATH_MSG_FATAL("Failed to enable multithreaded ROOT via PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   m_contextMaxFile.insert(std::pair<unsigned int, int>(IPoolSvc::kInputStream, m_dbAgeLimit));
   if (!connect(pool::ITransaction::READ).isSuccess()) {
      ATH_MSG_FATAL("Failed to connect Input PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   m_persistencySvcVec.push_back(psfactory->create("PersistencySvc", *m_catalog)); // Write Service
   if (m_persistencySvcVec[IPoolSvc::kOutputStream] == nullptr) {
      ATH_MSG_FATAL("Failed to create Output PersistencySvc.");
      return(StatusCode::FAILURE);
   }
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
StatusCode PoolSvc::stop() {
   unsigned int contextId = 0;
   bool retError = false;
   for (std::vector<pool::IPersistencySvc*>::const_iterator iter = m_persistencySvcVec.begin(),
		   last = m_persistencySvcVec.end(); iter != last; iter++, contextId++) {
      if (!disconnect(contextId).isSuccess()) {
         ATH_MSG_FATAL("Cannot disconnect Stream: " << contextId);
         retError = true;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::finalize() {
   unsigned int contextId = 0;
   for (std::vector<pool::IPersistencySvc*>::const_iterator iter = m_persistencySvcVec.begin(),
		   last = m_persistencySvcVec.end(); iter != last; iter++, contextId++) {
      delete *iter;
   }
   m_persistencySvcVec.clear();
   for (std::vector<CallMutex*>::const_iterator iter = m_pers_mut.begin(),
		   last = m_pers_mut.end(); iter != last; iter++) {
      delete *iter;
   }
   m_pers_mut.clear();
   if (m_catalog != nullptr) {
      m_catalog->commit();
      delete m_catalog; m_catalog = nullptr;
   }
   bool retError = false;
   if (!::AthService::finalize().isSuccess()) {
      ATH_MSG_FATAL("Cannot finalize AthService base class.");
      retError = true;
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::io_finalize() {
   ATH_MSG_INFO("I/O finalization...");
   unsigned int streamId = 0;
   for (std::vector<pool::IPersistencySvc*>::const_iterator iter = m_persistencySvcVec.begin(),
		   last = m_persistencySvcVec.end(); iter != last; iter++, streamId++) {
      ATH_MSG_DEBUG("Deleting PersSvc stream " << streamId);
      delete *iter;
   }
   m_persistencySvcVec.clear();
   if (m_catalog != nullptr) {
      m_catalog->commit();
      delete m_catalog; m_catalog = nullptr;
   }
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
const Token* PoolSvc::registerForWrite(const Placement* placement,
                                       const void* obj,
                                       const RootType& classDesc) const {
   std::lock_guard<CallMutex> lock(*m_pers_mut[IPoolSvc::kOutputStream]);
   Token* token = m_persistencySvcVec[IPoolSvc::kOutputStream]->registerForWrite(*placement, obj, classDesc);
   if (token == nullptr) {
      ATH_MSG_WARNING("Cannot write object: " << placement->containerName());
   }
   return(token);
}
//__________________________________________________________________________
void PoolSvc::setObjPtr(void*& obj, const Token* token) const {
   unsigned int contextId = IPoolSvc::kInputStream;
   const std::string& auxString = token->auxString();
   if (!auxString.empty()) {
      if (auxString.substr(0, 6) == "[CTXT=") {
         ::sscanf(auxString.c_str(), "[CTXT=%08X]", &contextId);
      } else if (auxString.substr(0, 8) == "[CLABEL=") {
         contextId = const_cast<PoolSvc*>(this)->getInputContext(auxString);
      }
      if (contextId >= m_persistencySvcVec.size()) {
         contextId = IPoolSvc::kInputStream;
      }
   }
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
unsigned int PoolSvc::getInputContext(const std::string& label, unsigned int maxFile) {
   std::lock_guard<CallMutex> lock(m_pool_mut);
   if (!label.empty()) {
      std::map<std::string, unsigned int>::const_iterator contextIter = m_contextLabel.find(label);
      if (contextIter != m_contextLabel.end()) {
         if (maxFile > 0) {
            m_contextMaxFile[contextIter->second] = maxFile;
         }
         return(contextIter->second);
      }
   }
   pool::IPersistencySvcFactory* psfactory = pool::IPersistencySvcFactory::get();
   if (psfactory == nullptr) {
      ATH_MSG_ERROR("Failed to create PersistencySvcFactory.");
      return(IPoolSvc::kInputStream);
   }
   const unsigned int id = m_persistencySvcVec.size();
   m_persistencySvcVec.push_back(psfactory->create("PersistencySvc", *m_catalog));
   if (m_persistencySvcVec[id] == nullptr) {
      ATH_MSG_ERROR("Failed to create Input PersistencySvc.");
      return(IPoolSvc::kInputStream);
   }
   m_pers_mut.push_back(new CallMutex);
   if (!connect(pool::ITransaction::READ).isSuccess()) {
      ATH_MSG_ERROR("Failed to connect Input PersistencySvc.");
      return(IPoolSvc::kInputStream);
   }
   if (!label.empty()) {
      m_contextLabel.insert(std::pair<std::string, unsigned int>(label, id));
   }
   m_contextMaxFile.insert(std::pair<unsigned int, int>(id, maxFile));
   return(id);
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
const pool::IFileCatalog* PoolSvc::catalog() const {
   return(m_catalog);
}
//__________________________________________________________________________
void PoolSvc::lookupBestPfn(const std::string& token, std::string& pfn, std::string& type) const {
   std::string dbID;
   pool::FCregister action;
   m_catalog->setAction(action);
   if (token.substr(0, 4) == "PFN:") {
      action.lookupFileByPFN(token.substr(4), dbID, type); // PFN -> FID
   } else if (token.substr(0, 4) == "LFN:") {
      action.lookupFileByLFN(token.substr(4), dbID); // LFN -> FID
   } else if (token.substr(0, 4) == "FID:") {
      dbID = token.substr(4);
   } else if (token.size() > Guid::null().toString().size()) { // full token
      Token tok;
      tok.fromString(token);
      dbID = tok.dbID().toString();
   } else { // guid only
      dbID = token;
   }
   action.lookupBestPFN(dbID, pool::FileCatalog::READ, pool::FileCatalog::SEQUENTIAL, pfn, type); // FID -> best PFN
}
//__________________________________________________________________________
void PoolSvc::renamePfn(const std::string& pf, const std::string& newpf) const {
   std::string dbID, type;
   pool::FCregister action;
   m_catalog->setAction(action);
   action.lookupFileByPFN(pf, dbID, type);
   if (dbID.empty()) {
      ATH_MSG_WARNING("Failed to lookup: " << pf << " in FileCatalog");
      return;
   }
   action.lookupFileByPFN(newpf, dbID, type);
   if (!dbID.empty()) {
      ATH_MSG_INFO("Found: " << newpf << " in FileCatalog");
      return;
   }
   action.renamePFN(pf, newpf);
}
//__________________________________________________________________________
pool::ICollection* PoolSvc::createCollection(const std::string& collectionType,
		const std::string& connection,
		const std::string& collectionName,
		const pool::ICollection::OpenMode& openMode,
		unsigned int contextId) const {
   std::string collection(collectionName);
   if (collectionType == "RootCollection") {
      if (collectionName.find("PFN:") == std::string::npos
	      && collectionName.find("LFN:") == std::string::npos
	      && collectionName.find("FID:") == std::string::npos) {
	 collection = "PFN:" + collectionName;
      }
   }
   std::lock_guard<CallMutex> lock(m_pool_mut);
   if (openMode != pool::ICollection::READ) {
      contextId = IPoolSvc::kOutputStream;
   } else {
      if (contextId > m_persistencySvcVec.size()) {
         contextId = IPoolSvc::kInputStream;
      } else if (contextId == m_persistencySvcVec.size()) {
         contextId = const_cast<PoolSvc*>(this)->getInputContext("");
      }
   }
   if (contextId >= m_persistencySvcVec.size()) {
      return(nullptr);
   }
   std::lock_guard<CallMutex> lockC(*m_pers_mut[contextId]);
   // Check POOL FileCatalog entry.
   bool insertFile = false;
   if (connection.substr(0, 4) == "PFN:") {
      pool::IFCAction action;
      m_catalog->setAction(action);
      std::string fid, fileType;
      action.lookupFileByPFN(connection.substr(4), fid, fileType);
      if (fid.empty()) { // No entry in file catalog
         insertFile = true;
         ATH_MSG_INFO("File is not in Catalog! Attempt to open it anyway.");
      }
   }
   // Check whether Collection Container exists.
   if (collectionType == "ImplicitCollection" || collectionType == "SeekableCollection") {
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
               pool::FCregister action;
               m_catalog->setAction(action);
               std::string fid = dbH->fid();
               action.registerPFN(connection.substr(4), "ROOT_All", fid);
            }
            return(nullptr); // no events
         }
      } catch(std::exception& e) {
         ATH_MSG_INFO("Failed to open container to check POOL collection - trying.");
      }
   }
   pool::CollectionFactory* collFac = pool::CollectionFactory::get();
   pool::CollectionDescription collDes(collection, collectionType, (collectionType == "ImplicitCollection" || collectionType == "SeekableCollection") ? connection : "");
   pool::ICollection* collPtr = nullptr;
   if (collectionType == "RootCollection" && contextId == IPoolSvc::kOutputStream) {
      ATH_MSG_INFO("Writing ExplicitROOT Collection - do not pass session pointer");
      collPtr = collFac->create(collDes, openMode);
   } else {
      try {
         collPtr = collFac->create(collDes, openMode, &m_persistencySvcVec[contextId]->session());
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
      } else if (openMode == pool::ICollection::READ && dbH->fid().empty()) {
         ATH_MSG_INFO("Cannot retrieve the FID of an existing POOL database: '"
                      << connection << "' - FileCatalog will NOT be updated.");
      } else {
         dbH->setTechnology(pool::ROOT_StorageType.type());
         pool::FCregister action;
         m_catalog->setAction(action);
         std::string fid = dbH->fid();
         action.registerPFN(connection.substr(4), "ROOT_All", fid);
      }
   }
   return(collPtr);
}
//__________________________________________________________________________
void PoolSvc::registerExistingCollection(pool::ICollection* coll, bool overwrite, bool sharedCat) {
   std::lock_guard<CallMutex> lock(m_pool_mut);
   pool::CollectionFactory* collFac = pool::CollectionFactory::get();
   m_catalog->commit();
   if (sharedCat) {
      collFac->registerExisting(coll, overwrite, m_catalog);
   } else {
      collFac->registerExisting(coll, overwrite, nullptr);
   }
   m_catalog->start();
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
StatusCode PoolSvc::connect(pool::ITransaction::Type type, unsigned int contextId) const {
   std::lock_guard<CallMutex> lock(m_pool_mut);
   if (type != pool::ITransaction::READ) {
      contextId = IPoolSvc::kOutputStream;
   } else {
      if (contextId > m_persistencySvcVec.size()) {
         contextId = IPoolSvc::kInputStream;
      } else if (contextId == m_persistencySvcVec.size()) {
         contextId = const_cast<PoolSvc*>(this)->getInputContext("");
      }
   }
   if (contextId >= m_persistencySvcVec.size()) {
      return(StatusCode::FAILURE);
   }
   std::lock_guard<CallMutex> lockC(*m_pers_mut[contextId]);
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
      unsigned int type = persSvc->session().transaction().type();
      if (!persSvc->session().transaction().commit()) {
         ATH_MSG_ERROR("POOL commit failed " << persSvc);
         persSvc->session().disconnectAll();
         return(StatusCode::FAILURE);
      }
      if (type == pool::ITransaction::READ) {
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
   if (persSvc->session().transaction().isActive()) {
      if (!persSvc->session().transaction().commitAndHold()) {
         ATH_MSG_ERROR("POOL commitAndHold failed " << persSvc);
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::disconnect(unsigned int contextId) const {
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
      const Guid guid(dbH->fid());
      m_guidLists[contextId].remove(guid);
   }
   dbH->disconnect();
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
long long int PoolSvc::getFileSize(const std::string& dbName, long tech, unsigned int contextId) const {
   std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, dbName);
   if (dbH == nullptr) {
      ATH_MSG_DEBUG("getFileSize: Failed to get Session/DatabaseHandle to get POOL FileSize property.");
      return 0; // failure
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      if (contextId == IPoolSvc::kOutputStream) {
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
      contextId = IPoolSvc::kInputStream;
   }
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
   std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, dbName);
   if (dbH == nullptr) {
      ATH_MSG_DEBUG("getAttribute: Failed to get Session/DatabaseHandle to get POOL property.");
      return(StatusCode::FAILURE);
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      if (contextId == IPoolSvc::kOutputStream) {
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
      contextId = IPoolSvc::kOutputStream;
   }
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
      contextId = IPoolSvc::kOutputStream;
   }
   std::lock_guard<CallMutex> lock(m_pool_mut);
   std::unique_ptr<pool::IDatabase> dbH = getDbHandle(contextId, dbName);
   if (dbH == nullptr) {
      ATH_MSG_DEBUG("Failed to get Session/DatabaseHandle to set POOL property.");
      return(StatusCode::FAILURE);
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      if (contextId == IPoolSvc::kOutputStream) {
         dbH->setTechnology(tech);
         dbH->connectForWrite();
      } else {
         dbH->connectForRead();
      }
   }
   bool retError = false;
   std::string objName;
   bool hasTTreeName = (contName.length() > 6 && contName.substr(0, 6) == "TTree=");
   if (contName.empty() || hasTTreeName || contextId != IPoolSvc::kOutputStream) {
      objName = hasTTreeName ? contName.substr(6) : contName;
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
      if (contName.find('(') != std::string::npos) {
         objName = contName.substr(contName.find('(') + 1); // Get BranchName between parenthesis
         objName = objName.substr(0, objName.find(')'));
      } else if (contName.find("::") != std::string::npos) {
         objName = contName.substr(contName.find("::") + 2); // Split off Tree name
      } else if (contName.find('_') != std::string::npos) {
         objName = contName.substr(contName.find('_') + 1); // Split off "POOLContainer"
         objName = objName.substr(0, objName.find('/')); // Split off key
      }
      std::string::size_type off = 0;
      while ((off = objName.find_first_of("<>/")) != std::string::npos) {
         objName.replace(off, 1, "_"); // Replace special chars (e.g. templates)
      }
      if (data[data.size() - 1] == 'L') {
         retError = contH->technologySpecificAttributes().setAttribute<long long int>(optName, atoll(data.c_str()), objName);
      } else {
         retError = contH->technologySpecificAttributes().setAttribute<int>(optName, atoi(data.c_str()), objName);
      }
      if (!retError) {
         ATH_MSG_DEBUG("Failed to set POOL container property, "
	         << optName
	         << " for "
	         << contName << " : " << objName
	         << " to "
	         << data);
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
   if (conn.find(":") == std::string::npos) {
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
	    if (pcon.substr(0, 9) == "frontier:") {
               physcons.push_back(pcon);
            }
         }
         delete dbset; dbset = nullptr;
      } else {
         ATH_MSG_DEBUG("setFrontierCache: Could not find any replicas for " << conn);
      }
   } else if (conn.substr(0, 9) == "frontier:") {
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
      ATH_MSG_DEBUG("Cache flag for connection "
	      << *iter
	      << " set to "
	      << webCache.webCacheInfo(*iter).isSchemaInfoCached());
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
pool::IFileCatalog* PoolSvc::createCatalog() {
   pool::IFileCatalog* ctlg = new pool::IFileCatalog;
   for (std::vector<std::string>::const_iterator iter = m_readCatalog.value().begin(),
	   last = m_readCatalog.value().end(); iter != last; ++iter) {
      ATH_MSG_DEBUG("POOL ReadCatalog is " << *iter);
      if (iter->substr(0, 8) == "apcfile:" || iter->substr(0, 7) == "prfile:") {
         std::string::size_type cpos = iter->find(":");
         // check for file accessed via ATLAS_POOLCOND_PATH
         std::string file = poolCondPath(iter->substr(cpos + 1));
         if (!file.empty()) {
            ATH_MSG_INFO("Resolved path (via ATLAS_POOLCOND_PATH) is " << file);
            ctlg->addReadCatalog("file:" + file);
         } else {
            // As backup, check for file accessed via PathResolver
            std::string file = PathResolver::find_file(iter->substr(cpos + 1), "DATAPATH");
            if (!file.empty()) {
               ATH_MSG_INFO("Resolved path (via DATAPATH) is " << file);
               ctlg->addReadCatalog("file:" + file);
            } else {
               ATH_MSG_WARNING("Unable to locate catalog for "
	               << *iter
	               << " check your ATLAS_POOLCOND_PATH and DATAPATH variables");
            }
         }
      } else {
         ctlg->addReadCatalog(*iter);
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
PoolSvc::PoolSvc(const std::string& name, ISvcLocator* pSvcLocator) :
        ::AthService(name, pSvcLocator),
	m_context(nullptr),
	m_catalog(nullptr),
	m_persistencySvcVec(),
	m_pers_mut(),
	m_contextLabel(),
	m_contextMaxFile(),
	m_guidLists() {
   declareProperty("WriteCatalog", m_writeCatalog = "xmlcatalog_file:PoolFileCatalog.xml");
   declareProperty("ReadCatalog", m_readCatalog);
   declareProperty("AttemptCatalogPatch", m_attemptCatalogPatch = true);
   declareProperty("ConnectionRetrialPeriod", m_retrialPeriod = 300);
   declareProperty("ConnectionRetrialTimeOut", m_retrialTimeOut = 3600);
   declareProperty("ConnectionTimeOut", m_timeOut = 5);
   declareProperty("ConnectionCleanUp", m_connClean = false);
   declareProperty("FrontierCompression", m_frontierComp = 5);
   declareProperty("FrontierRefreshSchema", m_frontierRefresh);
   declareProperty("FileOpen", m_fileOpen = "overwrite");
   declareProperty("MaxFilesOpen", m_dbAgeLimit = 0);
   declareProperty("SortReplicas", m_sortReplicas = true);
}
//__________________________________________________________________________
PoolSvc::~PoolSvc() {
}
//__________________________________________________________________________
std::unique_ptr<pool::IDatabase> PoolSvc::getDbHandle(unsigned int contextId, const std::string& dbName) const {
   pool::IDatabase* dbH = nullptr;
   if (contextId >= m_persistencySvcVec.size()) {
      contextId = IPoolSvc::kInputStream;
   }
   pool::ISession& sesH = m_persistencySvcVec[contextId]->session();
   if (!sesH.transaction().isActive()) {
      pool::ITransaction::Type transMode = pool::ITransaction::READ;
      if (contextId == IPoolSvc::kOutputStream) {
         transMode = pool::ITransaction::UPDATE;
      }
      if (!sesH.transaction().start(transMode)) {
         ATH_MSG_DEBUG("Failed to start transaction, type = " << transMode);
         return(nullptr);
      }
   }
   if (dbName.find("PFN:") == 0) {
      dbH = sesH.databaseHandle(dbName.substr(4), pool::DatabaseSpecification::PFN);
   } else if (dbName.find("LFN:") == 0) {
      dbH = sesH.databaseHandle(dbName.substr(4), pool::DatabaseSpecification::LFN);
   } else if (dbName.find("FID:") == 0) {
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
      testpath += "/" + leaf;
      struct stat stFileInfo;
      // try to retrieve file attribute - success indicates file exists
      if (stat(testpath.c_str(), &stFileInfo) == 0) {
         respath = testpath;
      }
   }
   return(respath);
}
