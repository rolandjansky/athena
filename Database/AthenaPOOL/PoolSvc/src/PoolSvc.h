/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOLSVC_H
#define POOLSVC_H

/** @file PoolSvc.h
 *  @brief This file contains the class definition for the PoolSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "PoolSvc/IPoolSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIoComponent.h"
#include "AthenaBaseComps/AthService.h"
#ifdef ATHENAHIVE
#include "tbb/recursive_mutex.h"
#endif

#include <string>
#include <vector>
#include <map>

// Forward declarations
namespace pool {
   class IContainer;
   class IDatabase;
   class IPersistencySvc;
   class ISession;
}
class IAthenaSealSvc;
class Guid;
class Placement;

template <class TYPE> class SvcFactory;

/** @class PoolSvc
 *  @brief This class provides the interface to the LCG POOL persistency software.
 **/
class PoolSvc : public ::AthService, virtual public IPoolSvc, virtual public IIoComponent {
   // Allow the factory class access to the constructor
   friend class SvcFactory<PoolSvc>;

public: // Non-static members
   /// Required of all Gaudi services:
   StatusCode initialize();
   StatusCode reinit();
   StatusCode io_reinit();
   /// Required of all Gaudi services:
   StatusCode stop();
   /// Required of all Gaudi services:
   StatusCode finalize();
   StatusCode io_finalize();
   /// Required of all Gaudi services:  see Gaudi documentation for details
   StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

   /// @return a token to a Data Object written to Pool.
   /// @param placement [IN] pointer to the placement hint.
   /// @param obj [IN] pointer to the Data Object to be written to Pool.
   /// @param classDesc [IN] pointer to the Seal class description for the Data Object.
   const Token* registerForWrite(const Placement* placement,
                                 const void* obj,
                                 const RootType& classDesc) const;

   /// @return void
   /// @param obj [OUT] pointer to the Data Object.
   /// @param token [IN] token of the Data Object for which a Pool Ref is filled.
   /// @param contextId [IN] id for PoolSvc persistency service to use for input.
   void setObjPtr(void*& obj,
	   const Token* token,
	   unsigned long contextId = IPoolSvc::kInputStream) const;

   /// @return an Id for an input context (POOL persistency service) and create it if needed.
   /// @param label [IN] string label to name new context and allow sharing (returns existing contextId)
   /// @param maxFile [IN] maximum number of open input files.
   unsigned long getInputContext(const std::string& label, unsigned int maxFile = 0);

   /// @return the context.
   const coral::Context* context() const;

   /// @return void
   /// @param compName [IN] string name of the component to be loaded.
   void loadComponent(const std::string& compName) const;

   /// @return the file catalog.
   const pool::IFileCatalog* catalog() const;

   /// @return void
   /// @param token [IN] filename/token string to be translated
   /// @param pfn [OUT] string PFN of database
   /// @param type [OUT] string filetype of database
   void lookupBestPfn(const std::string& token, std::string& pfn, std::string& type) const;

   /// @return void
   /// @param pf [IN] filename to be renamed
   /// @param newpf [IN] new filename
   void renamePfn(const std::string& pf, const std::string& newpf) const;

   /// @return a pointer to a Pool Collection.
   /// @param collectionType [IN] string containing the collection type.
   /// @param connection [IN] string containing the connection.
   /// @param collectionName [IN] string containing the persistent name of the collection.
   /// @param openMode [IN] ICollection open mode of the collection.
   /// @param contextId [IN] id for PoolSvc persistency service to use for input.
   pool::ICollection* createCollection(const std::string& collectionType,
	   const std::string& connection,
	   const std::string& collectionName,
	   const pool::ICollection::OpenMode& openMode = pool::ICollection::READ,
	   unsigned long contextId = IPoolSvc::kInputStream) const;

   /// @return void
   /// @param c [IN] collection to be registered
   /// @param overwrite [IN] whether to overwrite
   /// @param sharedCat [IN] whether to use same catalog as data
   void registerExistingCollection(pool::ICollection* c, bool overwrite, bool sharedCat = true);

   /// @return a token for a container entry.
   /// @param connection [IN] string containing the connection/file name.
   /// @param collection [IN] string containing the persistent name of the collection.
   /// @param ientry [IN] entry number for the token to be returned
   Token* getToken(const std::string& connection,
	   const std::string& collection,
	   const unsigned long ientry) const;

   /// Utility to test whether the dictionary knows about a given class.
   /// @param className [IN] string containing the name of the class to be checked.
   bool testDictionary(const std::string& className) const;

   /// Connect to a logical database unit; PersistencySvc is chosen according to transaction type (accessmode).
   StatusCode connect(pool::ITransaction::Type type,
	   unsigned long stream = IPoolSvc::kInputStream) const;

   /// Commit data for a given stream and flush buffer.
   /// @param stream [IN] poolStream to be commited.
   StatusCode commit(unsigned long stream = IPoolSvc::kInputStream) const;

   /// Commit data for a given stream and hold buffer.
   /// @param stream [IN] poolStream to be commited.
   StatusCode commitAndHold(unsigned long stream = IPoolSvc::kInputStream) const;

   /// Disconnect PersistencySvc associated with a stream.
   /// @param stream [IN] poolStream to be disconnected.
   StatusCode disconnect(unsigned long stream = IPoolSvc::kInputStream) const;

   /// Disconnect single Database.
   /// @param connection [IN] connection string for Database to be disconnected.
   /// @param contextId [IN] context id of database to be disconnected.
   StatusCode disconnectDb(const std::string& connection,
	   unsigned long contextId = IPoolSvc::kInputStream) const;

   /// Get POOL FileSize attribute for database without logging a message
   long long int getFileSize(const std::string& dbName, long tech, unsigned long contextId) const;

   /// Get POOL attributes - domain
   StatusCode getAttribute(const std::string& optName,
	   std::string& data,
	   long tech,
	   unsigned long contextId = IPoolSvc::kInputStream) const;

   /// Get POOL attributes - db/file, container/collection
   StatusCode getAttribute(const std::string& optName,
	   std::string& data,
	   long tech,
	   const std::string& dbName,
	   const std::string& contName = "",
	   unsigned long contextId = IPoolSvc::kInputStream) const;

   /// Set POOL attributes - domain
   StatusCode setAttribute(const std::string& optName,
	   const std::string& data,
	   long tech,
	   unsigned long contextId = IPoolSvc::kOutputStream) const;

   /// Set POOL attributes - db/file, container/collection
   StatusCode setAttribute(const std::string& optName,
	   const std::string& data,
	   long tech,
	   const std::string& dbName,
	   const std::string& contName = "",
	   unsigned long contextId = IPoolSvc::kOutputStream) const;

   /// Setup Frontier cache for given logical or physical connection name
   StatusCode setFrontierCache(const std::string& conn) const;

protected: // constructor and destructor
   /// Standard Service Constructor
   PoolSvc(const std::string& name, ISvcLocator* pSvcLocator);
   /// Destructor
   virtual ~PoolSvc();

private: // data
   coral::Context*                                   m_context;
   pool::IFileCatalog*                               m_catalog;
   ServiceHandle<IAthenaSealSvc>                     m_athenaSealSvc;
   std::vector<pool::IPersistencySvc*>               m_persistencySvcVec;
   std::map<std::string, unsigned long>              m_contextLabel;
   std::map<unsigned long, unsigned int>             m_contextMaxFile;
   mutable std::map<unsigned long, std::list<Guid> > m_guidLists;
   mutable std::map<std::string, std::vector<std::string> > m_containersMap;

private: // properties
   /// FileOpen, the open mode for the file ("append" or "overwrite").
   StringProperty  m_fileOpen;
   /// MaxFilesOpen, option to have PoolSvc limit the number of open Input Files: default = 0
   ///  (No files are closed automatically)
   IntegerProperty m_dbAgeLimit;
   /// WriteCatalog, the file catalog to be used to register output files (also default input catalog):
   ///	default = "" (use POOL default).
   StringProperty m_writeCatalog;
   /// ReadCatalog, the list of additional POOL input file catalogs to consult: default = empty vector.
   StringArrayProperty m_readCatalog;
   /// AttemptCatalogPatch, option to create catalog: default = false.
   BooleanProperty m_attemptCatalogPatch;
   /// ConnectionRetrialPeriod, retry period for CORAL Connection Service: default = 30 seconds
   IntegerProperty m_retrialPeriod;
   /// ConnectionRetrialTimeOut, the retrial time out for CORAL Connection Service: default = 300 seconds
   IntegerProperty m_retrialTimeOut;
   /// ConnectionTimeOut, the time out for CORAL Connection Service: default = 5 seconds
   IntegerProperty m_timeOut;
   /// ConnectionCleanUp - whether to use CORAL connection management thread: default = false.
   BooleanProperty m_connClean;
   /// Frontier proprties, compression level and list of schemas to be refreshed: default = 5
   IntegerProperty m_frontierComp;
   StringArrayProperty m_frontierRefresh;
   /// CheckDictionary, enable SEAL dictionary checking: default = true.
   BooleanProperty m_testDictionary;
   /// Use DBReplicaSvc to sort database connections, default = true.
   BooleanProperty m_sortReplicas;

private: // internal helper functions
   pool::IFileCatalog* createCatalog();

   // setup APR persistency
   StatusCode setupPersistencySvc();

   /// Get Session and Database handles
   StatusCode getSessionDbHandles(pool::ISession*& sesH,
	pool::IDatabase*& dbH,
	unsigned long contextId,
	const std::string& dbName) const;
   /// Get Container handle
   StatusCode getContainerHandle(pool::IDatabase* dbH,
	const std::string& contName,
	pool::IContainer*& contH,
	std::string& objName) const;

   /// Resolve a file using ATLAS_POOLCOND_PATH
   std::string poolCondPath(const std::string& leaf);

#ifdef ATHENAHIVE
  typedef tbb::recursive_mutex CallMutex;
  mutable CallMutex m_callLock;
#endif

};
#endif
