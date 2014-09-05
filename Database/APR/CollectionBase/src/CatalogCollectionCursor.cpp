/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionBase/CatalogCollectionCursor.h"
#include "CollectionBase/CollectionFactory.h"
#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"

#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/IFCContainer.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/FCException.h"

namespace pool { class ISession; }

#include "PersistentDataModel/Token.h"


pool::CatalogCollectionCursor::
CatalogCollectionCursor( pool::IFileCatalog& collectionCatalog,
			 std::string collectionLevelQuery,
			 std::string rowLevelQuery,
			 std::vector<std::string>* tokenOutputList,
			 std::vector<std::string>* attributeOutputList,
			 int rowCacheSize,
			 pool::ISession* session )
  : 
    m_collectionCatalog( collectionCatalog ),
    m_collectionLevelQuery( collectionLevelQuery ),
    m_rowLevelQuery( rowLevelQuery ),
    m_rowCacheSize( rowCacheSize ),
    m_session( session ),
    m_guidContainer(0),
    m_collection(0),
    m_query(0),
    m_cursor(0)
{
   if( tokenOutputList )   m_tokenOutputList = *tokenOutputList;
   if( attributeOutputList )   m_attribOutputList = *attributeOutputList;
   
  // Execute collection level query on collection catalog and get GUID's of collections in query result set.
  m_guidContainer = new pool::GuidContainer( &m_collectionCatalog );
  pool::FClookup act;
  m_collectionCatalog.setAction(act);
  m_collectionCatalog.start();
  act.lookupFileByQuery( m_collectionLevelQuery, *m_guidContainer );
  m_collectionCatalog.commit();
}


pool::CatalogCollectionCursor::~CatalogCollectionCursor()
{
  if( m_guidContainer ) 
  {
    delete m_guidContainer;
    m_guidContainer = 0;
  }

  if( m_query ) 
  {
    delete m_query;
    m_query = 0;
  }
}


bool 
pool::CatalogCollectionCursor::next()
{  
  if( m_cursor && m_cursor->next() )  {
    return true;
  }

  for(;;) {
     m_cursor = 0;
     delete m_query;
     m_query = 0;
     m_collection = 0;

     // Start a new collection catalog transaction.
     m_collectionCatalog.start();

     // Look for next collection in collection catalog query result set.
     if( m_guidContainer->hasNext() ) {
        // Get GUID of next collection.
        pool::FileCatalog::FileID guid = m_guidContainer->Next();
        m_collectionCatalog.commit();

        // Use GUID to access next collection from catalog.
        m_collection = pool::CollectionFactory::get()->openWithGuid( guid,
                                                                     &m_collectionCatalog,
                                                                     true,
                                                                     m_session );
        if( m_collection ) {
           // Execute row level query on this collection.
           m_query = m_collection->newQuery();
           m_query->setCondition( m_rowLevelQuery );
           if( m_tokenOutputList.size() )          {
              m_query->addToOutputList( m_tokenOutputList );
           }
           if( m_attribOutputList.size() )          {
              m_query->addToOutputList( m_attribOutputList );
           }
           m_query->setRowCacheSize( m_rowCacheSize );

           // Get cursor over query result set for this collection.
           // Note: Cursor object is deleted internally when query object is deleted.
           m_cursor = &( m_query->execute() );
           if( m_cursor->next() )	{
              return true;
           }
        } else {
           // MN: collection could not be opened?  how is the error handling done?
        }
     }
     else {
        // End of collection catalog query result set.
        m_collectionCatalog.commit();
        return false;
     }
  }
}


const pool::CollectionRowBuffer& 
pool::CatalogCollectionCursor::currentRow() const
{
  return m_cursor->currentRow();
}


const Token& 
pool::CatalogCollectionCursor::eventRef() const
{
  return m_cursor->eventRef();
}
 

void 
pool::CatalogCollectionCursor::close()
{
  m_cursor->close();
}      
