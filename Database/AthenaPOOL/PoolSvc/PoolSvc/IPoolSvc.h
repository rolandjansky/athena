/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOLSVC_IPOOLSVC_H
#define POOLSVC_IPOOLSVC_H

/** @file IPoolSvc.h
 *  @brief This file contains the class definition for the IPoolSvc interface class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "GaudiKernel/IService.h"
#include "CollectionBase/ICollection.h"
#include "PersistencySvc/ITransaction.h"
#include "DataModelRoot/RootType.h"

#include <string>

// Forward declarations
namespace pool {
   class IFileCatalog;
}
namespace coral {
   class Context;
}
class Placement;
class StatusCode;
class Token;

static const InterfaceID IID_IPoolSvc("IPoolSvc", 1, 0);

/** @class IPoolSvc
 *  @brief This class provides the interface to the LCG POOL persistency software.
 **/
class IPoolSvc : virtual public IService {

public: // static members
   /// Stream to distinguish the POOL Stream instances
   enum PoolStream { kInputStream, kOutputStream };

public: // Non-static members
   /// Retrieve interface ID
   static const InterfaceID& interfaceID() { return(IID_IPoolSvc); }

   /// @return a token to a Data Object written to Pool.
   /// @param placement [IN] pointer to the placement hint.
   /// @param obj [IN] pointer to the Data Object to be written to Pool.
   /// @param classDesc [IN] pointer to the Seal class description for the Data Object.
   virtual const Token* registerForWrite(const Placement* placement,
                                         const void* obj,
                                         const RootType& classDesc) const = 0;

   /// @return void
   /// @param obj [OUT] pointer to the Data Object.
   /// @param token [IN] token of the Data Object for which a Pool Ref is filled.
   virtual void setObjPtr(void*& obj, const Token* token) const = 0;

   /// @return an Id for an input context (POOL persistency service) and create it if needed.
   /// @param label [IN] string label to name new context and allow sharing (returns existing contextId)
   /// @param maxFile [IN] maximum number of open input files.
   virtual unsigned int getInputContext(const std::string& label, unsigned int maxFile = 0) = 0;

   /// @return the context.
   virtual const coral::Context* context() const = 0;

   /// @return void
   /// @param compName [IN] string name of the component to be loaded.
   virtual void loadComponent(const std::string& compName) const = 0;

   /// @return the file catalog.
   virtual const pool::IFileCatalog* catalog() const = 0;

   /// @return void
   /// @param token [IN] filename/token string to be translated
   /// @param pfn [OUT] string PFN of database
   /// @param type [OUT] string filetype of database
   virtual void lookupBestPfn(const std::string& token, std::string& pfn, std::string& type) const = 0;

   /// @return void
   /// @param pf [IN] filename to be renamed
   /// @param newpf [IN] new filename
   virtual void renamePfn(const std::string& pf, const std::string& newpf) const = 0;

   /// @return a pointer to a Pool Collection.
   /// @param collectionType [IN] string containing the collection type.
   /// @param connection [IN] string containing the connection.
   /// @param collectionName [IN] string containing the persistent name of the collection.
   /// @param openMode [IN] ICollection open mode of the collection.
   /// @param contextId [IN] id for PoolSvc persistency service to use for input.
   virtual pool::ICollection* createCollection(const std::string& collectionType,
	   const std::string& connection,
	   const std::string& collectionName,
	   const pool::ICollection::OpenMode& openMode = pool::ICollection::READ,
	   unsigned int contextId = IPoolSvc::kInputStream) const = 0;

   /// @return void
   /// @param c [IN] collection to be registered
   /// @param overwrite [IN] whether to overwrite
   /// @param sharedCat [IN] whether to use same catalog as data
   virtual void registerExistingCollection(pool::ICollection* c, bool overwrite, bool sharedCat = true) = 0;

   /// @return a token for a container entry.
   /// @param connection [IN] string containing the connection/file name.
   /// @param collection [IN] string containing the persistent name of the collection.
   /// @param ientry [IN] entry number for the token to be returned
   virtual Token* getToken(const std::string& connection,
	   const std::string& collection,
	   const unsigned long ientry) const = 0;

   /// Connect to a logical database unit; PersistencySvc is chosen according to transaction type (accessmode).
   virtual StatusCode connect(pool::ITransaction::Type type,
	   unsigned int contextId = IPoolSvc::kInputStream) const = 0;

   /// Commit data for a given stream and flush buffer.
   /// @param stream [IN] poolStream to be commited.
   virtual StatusCode commit(unsigned int contextId = IPoolSvc::kInputStream) const = 0;

   /// Commit data for a given stream and hold buffer.
   /// @param stream [IN] poolStream to be commited.
   virtual StatusCode commitAndHold(unsigned int contextId = IPoolSvc::kInputStream) const = 0;

   /// Disconnect PersistencySvc associated with a stream.
   /// @param stream [IN] poolStream to be disconnected.
   virtual StatusCode disconnect(unsigned int contextId = IPoolSvc::kInputStream) const = 0;

   /// Disconnect single Database.
   /// @param connection [IN] connection string for Database to be disconnected.
   /// @param contextId [IN] context id of database to be disconnected.
   virtual StatusCode disconnectDb(const std::string& connection,
	   unsigned int contextId = IPoolSvc::kInputStream) const = 0;

   /// Get POOL FileSize attribute for database without logging a message
   virtual long long int getFileSize(const std::string& dbName, long tech, unsigned int contextId) const = 0;

   /// Get POOL attributes - domain
   virtual StatusCode getAttribute(const std::string& optName,
	   std::string& data,
	   long tech,
	   unsigned int contextId = IPoolSvc::kInputStream) const = 0;

   /// Get POOL attributes - db/file, container/collection
   virtual StatusCode getAttribute(const std::string& optName,
	   std::string& data,
	   long tech,
	   const std::string& dbName,
	   const std::string& contName = "",
	   unsigned int contextId = IPoolSvc::kInputStream) const = 0;

   /// Set POOL attributes - domain
   virtual StatusCode setAttribute(const std::string& optName,
	   const std::string& data,
	   long tech,
	   unsigned int contextId = IPoolSvc::kOutputStream) const = 0;

   /// Set POOL attributes - db/file, container/collection
   virtual StatusCode setAttribute(const std::string& optName,
	   const std::string& data,
	   long tech,
	   const std::string& dbName,
	   const std::string& contName = "",
	   unsigned int contextId = IPoolSvc::kOutputStream) const = 0;

   /// Setup Frontier cache for given logical or physical connection name
   virtual StatusCode setFrontierCache(const std::string& conn) const = 0;
};

#endif
