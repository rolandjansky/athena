/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOLCOLLECTIONCONVERTER_H
#define POOLCOLLECTIONCONVERTER_H

/** @file PoolCollectionConverter.h
 *  @brief This file contains the class definition for the PoolCollectionConverter class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include <string>

// Forward declarations
class IPoolSvc;
class IMessageSvc;
namespace pool {
   class ICollection;
   class ICollectionQuery;
   class ICollectionCursor;
   class ICollectionMetadata;
}
class StatusCode;

/** @class PoolCollectionConverter
 *  @brief This class provides an interface to POOL collections.
 **/
class PoolCollectionConverter {

public:
   /// Constructor
   /// @param msgSvc [IN] pointer to the message service.
   /// @param collectionType [IN] type of the collection
   /// ("ExplicitROOT", "ExplicitMySQL", "ExplicitMySQLlt" or "ImplicitROOT").
   /// @param connection [IN] connection string, only needed for MySQL.
   /// @param inputCollection [IN] name of the collection.
   /// @param query [IN] query string.
   /// @param svc [IN] pointer to the PoolSvc.
   /// @param contextId [IN] id for PoolSvc persistency service to use for input.
   PoolCollectionConverter(IMessageSvc* msgSvc,
		   const std::string& collectionType,
		   const std::string& connection,
		   const std::string& inputCollection,
		   const std::string& query,
		   const IPoolSvc* svc);

   /// Destructor
   virtual ~PoolCollectionConverter();

   /// Required by all Gaudi Services
   StatusCode initialize();

   /// Disconnect Database
   StatusCode disconnectDb();

   /// Check whether has valid pool::ICollection*
   StatusCode isValid() const;

   /// @return ICollectionCursor over all entries
   pool::ICollectionCursor& selectAll() const;

   /// Query the collection
   /// @return ICollectionCursor over the results of the query
   pool::ICollectionCursor& executeQuery() const;

   /// @param it [IN] collection iterator.
   /// @param refName [IN] attribute name.
   std::string retrieveToken(const pool::ICollectionCursor* cursor, const std::string& refName) const;

   /// @return string map with all collection metadata.
   pool::ICollectionMetadata* retrieveMetadata() const;

private: // data
   IMessageSvc* m_msgSvc;
   std::string m_collectionType;
   std::string m_connection;
   std::string m_inputCollection;
   std::string m_query;
   const IPoolSvc* m_poolSvc;
   pool::ICollection* m_poolCollection;
   mutable pool::ICollectionQuery* m_collectionQuery;
   std::string m_inputContainer;

private: // hide copy and assignment
   PoolCollectionConverter(const PoolCollectionConverter& rhs);
   PoolCollectionConverter& operator=(const PoolCollectionConverter& rhs);
};
#endif
