/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONUTILITIES_COLLECTIONPOOL_H
#define COLLECTIONUTILITIES_COLLECTIONPOOL_H

#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"

#include <string>
#include <queue>


namespace pool
{

/* This is a utility class to prevent running out of memory when too many
   output collections are created.
   It keeps a "window" of open collections, closing "old" ones when a new
   one needs to be opened and reopening them on demand.
   Rows are cached up to a certain limit if the collection is not opened.

   @author Marcin.Nowak@cern.ch
*/

   
   class CollectionPool
   {
     typedef  std::vector< pool::CollectionRowBuffer >  rowVect_t;


  public:

     ///iterator that flushes and closes all collections
     class flushing_iterator {
    friend class CollectionPool;
    public:
       flushing_iterator( CollectionPool* cp )
	     : m_cpool( cp ), m_iter( cp->m_rowCache.begin() ), m_end( cp->m_rowCache.end() ),
	       m_forceOpen( false )
       {}
     
       void		forceOpen( bool fo=true ) { m_forceOpen = fo; }

       /// NOTE - the current collection can NOT be deleted before calling ++
       flushing_iterator& operator ++ ();
       bool		isValid()	{ return m_iter != m_end; }
       ICollection& 	operator * ()	{ return *operator->(); }
       ICollection* 	operator -> ();

    protected:
       CollectionPool* 	m_cpool;
       std::map< ICollection*, rowVect_t >::iterator  m_iter, m_end;
       bool		m_forceOpen;
     };



     CollectionPool( unsigned maxOpen=50, unsigned cacheSize=100 ) ;
     virtual ~CollectionPool();

     /// add GUID -> collection mapping
     virtual void addCollection( const std::string& guid, ICollection* coll );
     /// get back which collection corresponds to a GUID
     virtual ICollection* get( const std::string& guid );
     /// insert row to a connection designated for a given GUID
     virtual void insertRow( const std::string& guid , const pool::CollectionRowBuffer& row );

     virtual pool::CollectionRowBuffer&  getRowBuffer( const std::string& );
     virtual const std::string& getDstRefName();
 
  protected:
  
     void queueOpenColl( pool::ICollection* coll );
     void reduceQueue();
     void writeCache( pool::ICollection* coll );


     unsigned  		m_maxOpen;
     unsigned  		m_rowCacheSize;
     std::queue< pool::ICollection* > m_queue;
     std::map< std::string, pool::ICollection* > m_map;
     std::map< pool::ICollection*, rowVect_t > m_rowCache;
     pool::CollectionRowBuffer  m_rowBuffer;
     std::string   	m_dstRefName;
   };

}

#endif   

