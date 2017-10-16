/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file PoolSvc.cxx
 *  @brief This file contains the implementation for the PoolSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "PoolSvc.h"

#include "GaudiKernel/IIoComponentMgr.h"

#include "AthenaKernel/IAthenaSealSvc.h"

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
#include "RelationalAccess/IConnectionService.h"
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

#ifdef ATHENAHIVE
  #define HIVE_CALLMUTEX CallMutex::scoped_lock l(m_callLock)
#else
  #define HIVE_CALLMUTEX
#endif
  

//__________________________________________________________________________
StatusCode PoolSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Load AthenaSealSvc to check for type definitions
   if (!m_athenaSealSvc.retrieve().isSuccess()) {
      ATH_MSG_WARNING("Cannot get AthenaSealSvc - no dictionary checks will be done");
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
   if (m_context == 0) {
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
   int athLvl = msg().level();
   ATH_MSG_DEBUG("OutputLevel is " << athLvl);
   coral::MsgLevel lvl = coral::Warning;
   // Map gaudi print level to one of Seal.
   switch (athLvl) {
      case 1:  lvl = coral::Verbose; break;
      case 2:  lvl = coral::Debug;   break;
      case 3:  lvl = coral::Info;    break;
      case 4:  lvl = coral::Warning; break;
      case 5:  lvl = coral::Error;   break;
      default: lvl = coral::Warning;
   };
   coral::MessageStream::setMsgVerbosity(lvl);

   return setupPersistencySvc();
}


//__________________________________________________________________________
StatusCode PoolSvc::reinit()
{
   ATH_MSG_WARNING("reinit() called - that is not expected" << name());
   return StatusCode::SUCCESS;
}


//__________________________________________________________________________
StatusCode PoolSvc::io_reinit()
{
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
StatusCode PoolSvc::setupPersistencySvc()
{
   ATH_MSG_INFO("Setting up APR FileCatalog and Streams");
   if( m_catalog ) {
      m_catalog->commit();
      delete m_catalog;
   }
   m_catalog = createCatalog();
   if (m_catalog != 0) {
      m_catalog->start();
   } else {
      ATH_MSG_FATAL("Failed to setup POOL File Catalog.");
      return(StatusCode::FAILURE);
   }

   // Setup a persistency service
   unsigned int streamId = 0;
   for (std::vector<pool::IPersistencySvc*>::const_iterator iter = m_persistencySvcVec.begin(),
		   last = m_persistencySvcVec.end(); iter != last; iter++, streamId++) {
      delete *iter;
   }
   m_persistencySvcVec.clear();

   pool::IPersistencySvcFactory* psfactory = pool::IPersistencySvcFactory::get();
   if (psfactory == 0) {
      ATH_MSG_FATAL("Failed to create PersistencySvcFactory.");
      return(StatusCode::FAILURE);
   }
   
   m_persistencySvcVec.push_back(psfactory->create("PersistencySvc", *m_catalog)); // Read Service
   if (m_persistencySvcVec[IPoolSvc::kInputStream] == 0) {
      ATH_MSG_FATAL("Failed to create Input PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   m_contextMaxFile.insert(std::pair<unsigned long, int>(IPoolSvc::kInputStream, m_dbAgeLimit));
   if (!connect(pool::ITransaction::READ).isSuccess()) {
      ATH_MSG_FATAL("Failed to connect Input PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   m_persistencySvcVec.push_back(psfactory->create("PersistencySvc", *m_catalog)); // Write Service
   if (m_persistencySvcVec[IPoolSvc::kOutputStream] == 0) {
      ATH_MSG_FATAL("Failed to create Output PersistencySvc.");
      return(StatusCode::FAILURE);
   }
   pool::DatabaseConnectionPolicy policy;
   policy.setWriteModeForNonExisting(pool::DatabaseConnectionPolicy::CREATE);
   policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::OVERWRITE);
   if (m_fileOpen.value() == "update") {
      policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::UPDATE);
   }
   m_persistencySvcVec[IPoolSvc::kOutputStream]->session().setDefaultConnectionPolicy(policy);
   return StatusCode::SUCCESS;
}

//__________________________________________________________________________
StatusCode PoolSvc::stop() {
   unsigned int streamId = 0;
   bool retError = false;
   for (std::vector<pool::IPersistencySvc*>::const_iterator iter = m_persistencySvcVec.begin(),
		   last = m_persistencySvcVec.end(); iter != last; iter++, streamId++) {
      if (!disconnect(streamId).isSuccess()) {
         ATH_MSG_FATAL("Cannot disconnect Stream: " << streamId);
         retError = true;
      }
   }
   return(retError ? StatusCode::FAILURE : StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::finalize() {
   unsigned int streamId = 0;
   for (std::vector<pool::IPersistencySvc*>::const_iterator iter = m_persistencySvcVec.begin(),
		   last = m_persistencySvcVec.end(); iter != last; iter++, streamId++) {
      delete *iter;
   }
   m_persistencySvcVec.clear();
   if (m_catalog != 0) {
      m_catalog->commit();
      delete m_catalog; m_catalog = 0;
   }
   if (!this->io_finalize().isSuccess()) {
      ATH_MSG_WARNING("Cannot io_finalize.");
   }
   if (!m_athenaSealSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release AthenaSealSvc");
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
      ATH_MSG_DEBUG(" deleting PersSvc stream " << streamId );
      delete *iter;
   }
   m_persistencySvcVec.clear();
   if (m_catalog != 0) {
      m_catalog->commit();
      delete m_catalog; m_catalog = 0;
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
   Token* token = m_persistencySvcVec[IPoolSvc::kOutputStream]->registerForWrite(*placement, obj, classDesc);
   if (token == 0) {
      ATH_MSG_WARNING("Cannot write object: " << placement->containerName());
   }
   return(token);
}
//__________________________________________________________________________
void PoolSvc::setObjPtr(void*& obj, const Token* token, unsigned long contextId) const {
  HIVE_CALLMUTEX;

   if (contextId >= m_persistencySvcVec.size()) {
      contextId = IPoolSvc::kInputStream;
   }
   obj = m_persistencySvcVec[contextId]->readObject(*token, obj);
   std::map<unsigned long, unsigned int>::const_iterator maxFileIter = m_contextMaxFile.find(contextId);
   if (maxFileIter != m_contextMaxFile.end() && maxFileIter->second > 0) {
      m_guidLists[contextId].remove(token->dbID());
      m_guidLists[contextId].push_back(token->dbID());
      while (m_guidLists[contextId].size() > maxFileIter->second) {
         this->disconnectDb("FID:" + m_guidLists[contextId].begin()->toString(), contextId).ignore();
      }
   }
}
//__________________________________________________________________________
unsigned long PoolSvc::getInputContext(const std::string& label, unsigned int maxFile) {
   HIVE_CALLMUTEX;
   if (!label.empty()) {
      std::map<std::string, unsigned long>::const_iterator contextIter = m_contextLabel.find(label);
      if (contextIter != m_contextLabel.end()) {
         if (maxFile > 0) {
            m_contextMaxFile[contextIter->second] = maxFile;
         }
         return(contextIter->second);
      }
   }
   pool::IPersistencySvcFactory* psfactory = pool::IPersistencySvcFactory::get();
   if (psfactory == 0) {
      ATH_MSG_ERROR("Failed to create PersistencySvcFactory.");
      return(IPoolSvc::kInputStream);
   }
   const unsigned long id = m_persistencySvcVec.size();
   m_persistencySvcVec.push_back(psfactory->create("PersistencySvc", *m_catalog));
   if (m_persistencySvcVec[id] == 0) {
      ATH_MSG_ERROR("Failed to create Input PersistencySvc.");
      return(IPoolSvc::kInputStream);
   }
   if (!connect(pool::ITransaction::READ).isSuccess()) {
      ATH_MSG_ERROR("Failed to connect Input PersistencySvc.");
      return(IPoolSvc::kInputStream);
   }
   if (!label.empty()) {
      m_contextLabel.insert(std::pair<std::string, unsigned long>(label, id));
   }
   m_contextMaxFile.insert(std::pair<unsigned long, int>(id, maxFile));
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
		unsigned long contextId) const {
   HIVE_CALLMUTEX;

   std::string collection(collectionName);
   if (collectionType == "RootCollection") {
      if (collectionName.find("PFN:") == std::string::npos
	      && collectionName.find("LFN:") == std::string::npos
	      && collectionName.find("FID:") == std::string::npos) {
	 collection = "PFN:" + collectionName;
      }
   }
   if (contextId >= m_persistencySvcVec.size()) {
      contextId = IPoolSvc::kInputStream;
   }
   if (contextId == IPoolSvc::kInputStream && openMode != pool::ICollection::READ) {
      contextId = IPoolSvc::kOutputStream;
   }
   if (contextId >= m_persistencySvcVec.size() ) {
      ATH_MSG_WARNING("CreateCollection '" << collectionName << "' for stream #"
                      << contextId << " but " << name() << " has only " <<
                      m_persistencySvcVec.size() << " streams open");
      return nullptr;
   }
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
      pool::ISession* sesH = 0;
      pool::IDatabase* dbH = 0;
      if (!getSessionDbHandles(sesH, dbH, contextId, connection).isSuccess()) {
         ATH_MSG_INFO("Failed to get Session/DatabaseHandle to create POOL collection.");
         delete dbH; dbH = 0;
         return(0);
      }
      try {
         if (dbH->openMode() == pool::IDatabase::CLOSED) {
            dbH->connectForRead();
         }
         std::map<unsigned long, unsigned int>::const_iterator maxFileIter = m_contextMaxFile.find(contextId);
         if (maxFileIter != m_contextMaxFile.end() && maxFileIter->second > 0 && !dbH->fid().empty()) {
            const Guid guid(dbH->fid());
            m_guidLists[contextId].remove(guid);
            m_guidLists[contextId].push_back(guid);
            while (m_guidLists[contextId].size() > maxFileIter->second + 1) {
               this->disconnectDb("FID:" + m_guidLists[contextId].begin()->toString(), contextId).ignore();
            }
         }
         std::map<std::string, std::vector<std::string> >::const_iterator it = m_containersMap.find(dbH->fid());
         if (it == m_containersMap.end()) {
            it = m_containersMap.insert(std::pair<std::string, std::vector<std::string> >(dbH->fid(), dbH->containers())).first;
         }
         if (find(it->second.begin(), it->second.end(), collection) == it->second.end()) {
            ATH_MSG_INFO("Failed to find container " << collection << " to create POOL collection.");
            if (insertFile && m_attemptCatalogPatch.value()) {
               dbH->setTechnology(pool::ROOT_StorageType.type());
               pool::FCregister action;
               m_catalog->setAction(action);
               std::string fid = dbH->fid();
               action.registerPFN(connection.substr(4), "ROOT_All", fid);
            }
            delete dbH; dbH = 0;
            return(0); // no events
         }
      } catch(std::exception& e) {
         ATH_MSG_INFO("Failed to open container to check POOL collection - trying.");
      }
      delete dbH; dbH = 0;
   }
   pool::CollectionFactory* collFac = pool::CollectionFactory::get();
   pool::CollectionDescription collDes(collection, collectionType, (collectionType == "ImplicitCollection" || collectionType == "SeekableCollection") ? connection : "");
   pool::ICollection* collPtr = 0;
   if (collectionType == "RootCollection" && contextId == IPoolSvc::kOutputStream) {
      ATH_MSG_INFO("Writing ExplicitROOT Collection - do not pass session pointer");
      collPtr = collFac->create(collDes, openMode);
   } else {
      try {
         collPtr = collFac->create(collDes, openMode, &m_persistencySvcVec[contextId]->session());
      } catch (std::exception &e) {
         if (insertFile) {
            pool::ISession* sesH = 0;
            pool::IDatabase* dbH = 0;
            if (getSessionDbHandles(sesH, dbH, contextId, connection).isSuccess()) {
               if (!dbH->fid().empty()) {
                  delete dbH; dbH = 0;
                  return(0); // no events
               }
            }
            delete dbH; dbH = 0;
         }
         throw; // bad file, rethrow
      }
   }
   if (insertFile && m_attemptCatalogPatch.value()) {
      pool::ISession* sesH = 0;
      pool::IDatabase* dbH = 0;
      if (!getSessionDbHandles(sesH, dbH, contextId, connection).isSuccess()) {
         ATH_MSG_INFO("Failed to create FileCatalog entry.");
         delete dbH; dbH = 0;
         return(collPtr);
      }
      dbH->setTechnology(pool::ROOT_StorageType.type());
      pool::FCregister action;
      m_catalog->setAction(action);
      std::string fid = dbH->fid();
      action.registerPFN(connection.substr(4), "ROOT_All", fid);
      delete dbH; dbH = 0;
   }
   return(collPtr);
}
//__________________________________________________________________________
void PoolSvc::registerExistingCollection(pool::ICollection* coll, bool overwrite, bool sharedCat) {
   HIVE_CALLMUTEX;

   pool::CollectionFactory* collFac = pool::CollectionFactory::get();
   m_catalog->commit();
   if (sharedCat) {
      collFac->registerExisting(coll, overwrite, m_catalog);
   } else {
      collFac->registerExisting(coll, overwrite, 0);
   }
   m_catalog->start();
}
//__________________________________________________________________________
Token* PoolSvc::getToken(const std::string& connection,
	const std::string& collection,
	const unsigned long ientry) const {
   HIVE_CALLMUTEX;
   pool::ISession* sesH = 0;
   pool::IDatabase* dbH = 0;
   if (!getSessionDbHandles(sesH, dbH, IPoolSvc::kInputStream, connection).isSuccess()) {
      delete dbH; dbH = 0;
      return(0);
   }
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      dbH->connectForRead();
   }
   std::map<std::string, std::vector<std::string> >::const_iterator it = m_containersMap.find(dbH->fid());
   if (it == m_containersMap.end()) {
      it = m_containersMap.insert(std::pair<std::string, std::vector<std::string> >(dbH->fid(), dbH->containers())).first;
   }
   if (find(it->second.begin(), it->second.end(), collection) ==  it->second.end()) {
      ATH_MSG_INFO("Failed to find container " << collection << " to get Token.");
      delete dbH; dbH = 0;
      return(0);
   }
   pool::IContainer* contH = 0;
   std::string objName;
   if (!getContainerHandle(dbH, collection, contH, objName).isSuccess()) {
      delete contH; contH = 0;
      delete dbH; dbH = 0;
      return(0);
   }
   pool::ITokenIterator* tokenIter = contH->tokens("");
   Token* thisToken = tokenIter->next();
   for (unsigned long ipos = 0; ipos < ientry; ipos++) {
      delete thisToken; thisToken = tokenIter->next();
   }
   delete tokenIter; tokenIter = 0;
   delete contH; contH = 0;
   delete dbH; dbH = 0;
   return(thisToken);
}
//__________________________________________________________________________
bool PoolSvc::testDictionary(const std::string& className) const {
   if (m_athenaSealSvc != 0 && m_testDictionary.value()) {
      return(m_athenaSealSvc->checkClass(className).isSuccess());
   }
   return(true);
}
//__________________________________________________________________________
StatusCode PoolSvc::connect(pool::ITransaction::Type type, unsigned long stream) const {
   HIVE_CALLMUTEX;
   if (type != pool::ITransaction::READ) {
      stream = IPoolSvc::kOutputStream;
   }
   if (stream >= m_persistencySvcVec.size() ) {
      ATH_MSG_WARNING("connect() for stream #" << stream << " but " << name() << " has only "
                      << m_persistencySvcVec.size() << " streams open");
     return(StatusCode::FAILURE);
   }
   pool::IPersistencySvc* persSvc = m_persistencySvcVec[stream];
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
StatusCode PoolSvc::commit(unsigned long stream) const {
   HIVE_CALLMUTEX;

   pool::IPersistencySvc* persSvc = m_persistencySvcVec[stream];
   if (persSvc != 0 && persSvc->session().transaction().isActive()) {
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
StatusCode PoolSvc::commitAndHold(unsigned long stream) const {
   HIVE_CALLMUTEX;

   pool::IPersistencySvc* persSvc = m_persistencySvcVec[stream];
   if (persSvc->session().transaction().isActive()) {
      if (!persSvc->session().transaction().commitAndHold()) {
         ATH_MSG_ERROR("POOL commitAndHold failed " << persSvc);
         return(StatusCode::FAILURE);
      }
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::disconnect(unsigned long stream) const {
   HIVE_CALLMUTEX;

   if (stream >= m_persistencySvcVec.size()) {
      return(StatusCode::SUCCESS);
   }
   pool::IPersistencySvc* persSvc = m_persistencySvcVec[stream];
   if (persSvc != 0 && persSvc->session().transaction().isActive()) {
      if (!commit(stream).isSuccess()) {
         ATH_MSG_ERROR("disconnect failed to commit " << persSvc);
         return(StatusCode::FAILURE);
      }
      persSvc->session().disconnectAll();
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::disconnectDb(const std::string& connection, unsigned long contextId) const {
   HIVE_CALLMUTEX;
   pool::ISession* sesH = 0;
   pool::IDatabase* dbH = 0;
   if (contextId >= m_persistencySvcVec.size()) {
      contextId = IPoolSvc::kInputStream;
   }
   if (contextId >= m_persistencySvcVec.size()) {
      return(StatusCode::SUCCESS);
   }
   if (!getSessionDbHandles(sesH, dbH, contextId, connection).isSuccess()) {
      ATH_MSG_ERROR("Failed to get Session/DatabaseHandle.");
      delete dbH; dbH = 0;
      return(StatusCode::FAILURE);
   }
   std::map<unsigned long, unsigned int>::const_iterator maxFileIter = m_contextMaxFile.find(contextId);
   if (maxFileIter != m_contextMaxFile.end() && maxFileIter->second > 0) {
      const Guid guid(dbH->fid());
      m_guidLists[contextId].remove(guid);
   }
   m_containersMap.erase(dbH->fid());
   dbH->disconnect();
   delete dbH; dbH = 0;
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
long long int PoolSvc::getFileSize(const std::string& dbName, long tech, unsigned long contextId) const {
   pool::ISession* sesH = 0;
   pool::IDatabase* dbH = 0;
   if (!getSessionDbHandles(sesH, dbH, contextId, dbName).isSuccess()) {
      ATH_MSG_DEBUG("getFileSize: Failed to get Session/DatabaseHandle to get POOL FileSize property.");
      delete dbH; dbH = 0;
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
   long long int value = dbH->technologySpecificAttributes().attribute<long long int>("FILE_SIZE");
   delete dbH; dbH = 0;
   return(value);
}
//_______________________________________________________________________
StatusCode PoolSvc::getAttribute(const std::string& optName,
		std::string& data,
		long tech,
		unsigned long contextId) const {
   if (contextId >= m_persistencySvcVec.size()) {
      contextId = IPoolSvc::kInputStream;
   }
   pool::ISession* sesH = &m_persistencySvcVec[contextId]->session();
   if (sesH == 0) {
       ATH_MSG_DEBUG("Failed to get SessionHandle for context " << contextId << " to get POOL property.");
      return(StatusCode::FAILURE);
   }
   std::ostringstream oss;
   if (data == "DbLonglong") {
      long long int value = sesH->technologySpecificAttributes(tech).attribute<long long int>(optName);
      oss << std::dec << value;
   } else if (data == "double") {
      double value = sesH->technologySpecificAttributes(tech).attribute<double>(optName);
      oss << std::dec << value;
   } else {
      int value = sesH->technologySpecificAttributes(tech).attribute<int>(optName);
      oss << std::dec << value;
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
		unsigned long contextId) const {
   pool::ISession* sesH = 0;
   pool::IDatabase* dbH = 0;
   if (!getSessionDbHandles(sesH, dbH, contextId, dbName).isSuccess()) {
      ATH_MSG_DEBUG("getAttribute: Failed to get Session/DatabaseHandle to get POOL property.");
      delete dbH; dbH = 0;
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
         long long int value = dbH->technologySpecificAttributes().attribute<long long int>(optName);
         oss << std::dec << value;
      } else if (data == "double") {
         double value = dbH->technologySpecificAttributes().attribute<double>(optName);
         oss << std::dec << value;
      } else if (data == "string") {
         char* value = dbH->technologySpecificAttributes().attribute<char*>(optName);
         oss << value;
      } else {
         int value = dbH->technologySpecificAttributes().attribute<int>(optName);
         oss << std::dec << value;
      }
      ATH_MSG_INFO("Database (" << dbH->pfn() << ") attribute [" << optName << "]" << ": " << oss.str());
   } else {
      pool::IContainer* contH = 0;
      std::string objName;
      if (!getContainerHandle(dbH, contName, contH, objName).isSuccess()) {
         ATH_MSG_DEBUG("Failed to get ContainerHandle to get POOL property.");
         delete contH; contH = 0;
         delete dbH; dbH = 0;
         return(StatusCode::FAILURE);
      }
      if (data == "DbLonglong") {
         long long int value = contH->technologySpecificAttributes().attribute<long long int>(optName);
         oss << std::dec << value;
      } else if (data == "double") {
         double value = contH->technologySpecificAttributes().attribute<double>(optName);
         oss << std::dec << value;
      } else {
         int value = contH->technologySpecificAttributes().attribute<int>(optName);
         oss << std::dec << value;
      }
      delete contH; contH = 0;
      ATH_MSG_INFO("Container attribute [" << contName << "." << optName << "]: " << oss.str());
   }
   data = oss.str();
   delete dbH; dbH = 0;
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode PoolSvc::setAttribute(const std::string& optName,
		const std::string& data,
		long tech,
		unsigned long contextId) const {
   HIVE_CALLMUTEX;

   if (contextId >= m_persistencySvcVec.size()) {
      contextId = IPoolSvc::kOutputStream;
   }
   pool::ISession* sesH = &m_persistencySvcVec[contextId]->session();
   if (sesH == 0) {
      ATH_MSG_DEBUG("Failed to get SessionHandle to set POOL property.");
      return(StatusCode::FAILURE);
   }
   if (data[data.size() - 1] == 'L') {
      long long int value = atoll(data.c_str());
      if (!sesH->technologySpecificAttributes(tech).setAttribute<long long int>(optName, value)) {
         ATH_MSG_DEBUG("Failed to set POOL property, " << optName << " to " << data);
         return(StatusCode::FAILURE);
      }
   } else {
      int value = atoi(data.c_str());
      if (!sesH->technologySpecificAttributes(tech).setAttribute<int>(optName, value)) {
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
		unsigned long contextId) const {
   HIVE_CALLMUTEX;

   pool::ISession* sesH = 0;
   pool::IDatabase* dbH = 0;
   if (!getSessionDbHandles(sesH, dbH, contextId, dbName).isSuccess()) {
      ATH_MSG_DEBUG("Failed to get Session/DatabaseHandle to set POOL property.");
      delete dbH; dbH = 0;
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
         long long int value = atoll(data.c_str());
         retError = dbH->technologySpecificAttributes().setAttribute<long long int>(optName, value, objName);
      } else {
         int value = atoi(data.c_str());
         retError = dbH->technologySpecificAttributes().setAttribute<int>(optName, value, objName);
      }
      if (!retError) {
         ATH_MSG_DEBUG("Failed to set POOL property, " << optName << " to " << data);
         delete dbH; dbH = 0;
         return(StatusCode::FAILURE);
      }
   } else {
      pool::IContainer* contH = 0;
      if (!getContainerHandle(dbH, contName, contH, objName).isSuccess()) {
         ATH_MSG_DEBUG("Failed to get ContainerHandle to set POOL property.");
         delete contH; contH = 0;
         delete dbH; dbH = 0;
         return(StatusCode::FAILURE);
      }
      if (data[data.size() - 1] == 'L') {
         long long int value = atoll(data.c_str());
         retError = contH->technologySpecificAttributes().setAttribute<long long int>(optName,
	         value,
	         objName);
      } else {
         int value = atoi(data.c_str());
         retError = contH->technologySpecificAttributes().setAttribute<int>(optName, value, objName);
      }
      if (!retError) {
         ATH_MSG_DEBUG("Failed to set POOL container property, "
	         << optName
	         << " for "
	         << contName << " : " << objName
	         << " to "
	         << data);
         delete contH; contH = 0;
         delete dbH; dbH = 0;
         return(StatusCode::FAILURE);
      }
      delete contH; contH = 0;
   }
   delete dbH; dbH = 0;
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::setFrontierCache(const std::string& conn) const {
   HIVE_CALLMUTEX;

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
      if (dbset != 0) {
         for (int irep = 0, nrep = dbset->numberOfReplicas(); irep < nrep; ++irep) {
	    const std::string pcon = dbset->replica(irep).connectionString();
	    if (pcon.substr(0, 9) == "frontier:") {
               physcons.push_back(pcon);
            }
         }
         delete dbset; dbset = 0;
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
   HIVE_CALLMUTEX;

   pool::IFileCatalog* ctlg = new pool::IFileCatalog;
   for (std::vector<std::string>::const_iterator iter = m_readCatalog.value().begin(),
	   last = m_readCatalog.value().end(); iter != last; ++iter) {
      ATH_MSG_DEBUG("POOL ReadCatalog is " << *iter);
      if (iter->substr(0, 7) == "prfile:") {
         std::string file = PathResolver::find_file(iter->substr(7), "DATAPATH");
         if (file.empty()) {
            ATH_MSG_WARNING("Unable to locate catalog for "
	            << *iter
	            << " via PathResolver, check your DATAPATH variable");
         } else {
            ATH_MSG_INFO("Resolved path (via DATAPATH) is " << file);
            ctlg->addReadCatalog("file:" + file);
         }
      } else if (iter->substr(0, 8) == "apcfile:") {
         // check for file accessed via ATLAS_POOLCOND_PATH
         std::string file = poolCondPath(iter->substr(8));
         if (!file.empty()) {
            ATH_MSG_INFO("Resolved path (via ATLAS_POOLCOND_PATH) is " << file);
            ctlg->addReadCatalog("file:" + file);
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
      return(0); // This catalog is not setup properly!
   }
   return(ctlg);
}
//__________________________________________________________________________
PoolSvc::PoolSvc(const std::string& name, ISvcLocator* pSvcLocator) :
        ::AthService(name, pSvcLocator),
	m_context(0),
	m_catalog(0),
	m_athenaSealSvc("AthenaSealSvc", name),
	m_persistencySvcVec(),
	m_contextLabel(),
	m_contextMaxFile(),
	m_guidLists(),
	m_containersMap() {
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
   declareProperty("CheckDictionary", m_testDictionary = true);
   declareProperty("SortReplicas", m_sortReplicas = true);
}
//__________________________________________________________________________
PoolSvc::~PoolSvc() {
}
//__________________________________________________________________________
StatusCode PoolSvc::getSessionDbHandles(pool::ISession*& sesH,
		pool::IDatabase*& dbH,
		unsigned long contextId,
		const std::string& dbName) const {
   if (contextId >= m_persistencySvcVec.size()) {
      contextId = IPoolSvc::kInputStream;
   }
   sesH = &m_persistencySvcVec[contextId]->session();
   if (sesH == 0) {
      ATH_MSG_DEBUG("Failed to get SessionHandle.");
      return(StatusCode::FAILURE);
   }
   if (!sesH->transaction().isActive()) {
      pool::ITransaction::Type transMode = pool::ITransaction::READ;
      if (contextId == IPoolSvc::kOutputStream) {
         transMode = pool::ITransaction::UPDATE;
      }
      if (!sesH->transaction().start(transMode)) {
         ATH_MSG_DEBUG("Failed to start transaction, type = " << transMode);
         return(StatusCode::FAILURE);
      }
   }
   if (dbName.find("PFN:") == 0) {
      dbH = sesH->databaseHandle(dbName.substr(4), pool::DatabaseSpecification::PFN);
   } else if (dbName.find("LFN:") == 0) {
      dbH = sesH->databaseHandle(dbName.substr(4), pool::DatabaseSpecification::LFN);
   } else if (dbName.find("FID:") == 0) {
      dbH = sesH->databaseHandle(dbName.substr(4), pool::DatabaseSpecification::FID);
   } else {
      dbH = sesH->databaseHandle(dbName, pool::DatabaseSpecification::PFN);
   }
   if (dbH == 0) {
      ATH_MSG_DEBUG("Failed to get DatabaseHandle.");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode PoolSvc::getContainerHandle(pool::IDatabase* dbH,
		const std::string& contName,
		pool::IContainer*& contH,
		std::string& objName) const {
   if (dbH->openMode() == pool::IDatabase::CLOSED) {
      dbH->connectForRead();
   }
   if (contName.find("DataHeader") != std::string::npos) {
      contH = dbH->containerHandle(contName.substr(0, contName.find("_p")));
   } else {
      contH = dbH->containerHandle(contName);
   }
   if (contH == 0) {
      ATH_MSG_DEBUG("Failed to get ContainerHandle for: " << contName);
      return(StatusCode::FAILURE);
   }
   if (contName.find('(') != std::string::npos) {
      objName = contName.substr(contName.find('(') + 1); // Get BranchName between parenthesis
      objName = objName.substr(0, objName.find(')'));
   } else if (contName.find("::") != std::string::npos) {
      objName = contName.substr(contName.find("::") + 2); // Split off Tree name
   } else {
      objName = contName.substr(contName.find('_') + 1); // Split off "POOLContainer"
      objName = objName.substr(0, objName.find('/')); // Split off key
   }
   std::string::size_type off = 0;
   while ((off = objName.find_first_of("<>/")) != std::string::npos) {
      objName.replace(off, 1, "_"); // Replace special chars (e.g. templates)
   }
   return(StatusCode::SUCCESS);
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
