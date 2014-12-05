/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_CATALOGCOLLECTIONCURSOR_H
#define COLLECTIONBASE_CATALOGCOLLECTIONCURSOR_H

#include "CollectionBase/ICollectionCursor.h"

#include <string>
#include <vector>


namespace pool { 
  
  class ISession;
  class GuidContainer;
  class ICollection;
  class IFileCatalog;
  class ICollectionQuery;

  /**
   * @class CatalogCollectionCursor CatalogCollectionCursor.h CollectionBase/CatalogCollectionCursor.h
   *
   * A cursor used to navigate over all collections registered in a collection catalog.
   * Queries are possible using both collection level and row level metadata.
   */ 
  class CatalogCollectionCursor : public ICollectionCursor 
  {
  public:
    /**
     * Constructor.
     *
     * @param catalog Collection catalog manager.
     * @param collectionLevelQuery Predicates for collection level queries.
     * @param rowLevelQuery Predicates for row level queries.
     * @param tokenOutputList Names of Token columns to be retrieved by row query.
     * @param attribOutputList Names of attribute columns to be retrieved by row query.
     * @param rowCacheSize Size of cache used to store rows retrieved by row query.
     * @param session Database session.
     */  
    CatalogCollectionCursor( IFileCatalog& catalog,
                             std::string collectionLevelQuery = "",
		             std::string rowLevelQuery = "",
                             std::vector<std::string>* tokenOutputList = 0,
                             std::vector<std::string>* attribOutputList = 0,
                             int rowCacheSize = 0,
                             ISession* session = 0 );

    /// Default destructor.
    virtual ~CatalogCollectionCursor();

    /// Disable copy ctor, assignment.
    CatalogCollectionCursor (const CatalogCollectionCursor&) = delete;
    CatalogCollectionCursor& operator= (const CatalogCollectionCursor&) = delete;

    /// Advances the cursor to the next row of the query result set of the current collection.
    virtual bool next();

    /// Returns the event reference and associated metadata of the row pointed to by the cursor.
    virtual const CollectionRowBuffer& currentRow() const;

    /// Returns the event reference Token.
    virtual const Token& eventRef() const;

    /// Cleanup.
    virtual void close();
       
  private:
    /// Collection catalog manager.
    IFileCatalog& m_collectionCatalog;

    /// Predicates for collection level query.
    std::string m_collectionLevelQuery;

    /// Predicates for row level query.
    std::string m_rowLevelQuery;

    /// Names of Token columns to be retreived by row query.
    std::vector<std::string> m_tokenOutputList;

    /// Names of attribute columns to be retreived by row query.
    std::vector<std::string> m_attribOutputList;

    /// Size of cache used to store rows retrieved by row query.
    int m_rowCacheSize;

    /// Database session.
    ISession* m_session;

    /// Container of collection catalog GUID's returned by query.
    GuidContainer* m_guidContainer;

    /// Current collection.
    ICollection* m_collection;

    /// Query object for current collection.
    ICollectionQuery* m_query;

    /// Current position in current collection.
    ICollectionCursor* m_cursor;
  };
  
}

#endif

