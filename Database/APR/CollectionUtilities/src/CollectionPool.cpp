/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionUtilities/CollectionPool.h"
#include "CollectionBase/ICollectionDataEditor.h"
#include "CollectionBase/CollectionDescription.h"

#include <stdexcept>

using namespace pool;
using namespace std;


CollectionPool::CollectionPool( unsigned maxOpen, unsigned cacheSize)  {
   m_maxOpen = ( maxOpen>1? maxOpen : 2 );
   m_rowCacheSize = cacheSize;
}

CollectionPool::~CollectionPool() {
   flushing_iterator i( this );
   while( i.isValid() ) {
      ICollection *coll = &*i;
      ++i;
      // order important - delete after moving to the next
      delete coll;
   }
}
     
       
void  CollectionPool::addCollection( const std::string& guid, ICollection* coll ) {
   if( m_map.find(guid) != m_map.end() )
      throw std::runtime_error("Attempt to overwrite GUID in collections map");
   m_map[guid] = coll;
   rowVect_t& row = m_rowCache[ coll ];
   if (row.capacity() == 0) {
      // new collection
      row.reserve( m_rowCacheSize/2+1);
      if( coll->isOpen() ) {
	 queueOpenColl( coll );
      }
   }
}


ICollection*  CollectionPool::get( const std::string& guid ) {
   ICollection* coll = m_map[guid];
   if( !coll->isOpen() ) {
      coll->open();
      queueOpenColl( coll );
   }
   return coll;
}

void  CollectionPool::insertRow( const std::string& guid , const pool::CollectionRowBuffer& row ) {
   ICollection* coll = m_map[guid];
   if( coll->isOpen() ) {
      coll->dataEditor().insertRow( row );
   } else {
      rowVect_t &rowVect = m_rowCache[ coll ];
      rowVect.push_back( row );
      if( rowVect.size() >= m_rowCacheSize ) {
	 coll->open();
	 writeCache( coll );
	 queueOpenColl( coll );
      }
   }
}

pool::CollectionRowBuffer&   CollectionPool::getRowBuffer( const std::string&  ) {
   return m_rowBuffer;
}

const std::string&  CollectionPool::getDstRefName() {
   return m_dstRefName;
}


void  CollectionPool::queueOpenColl( pool::ICollection* coll ) {
   if( m_queue.empty() ) {
      // first open collection in - store a rowBuffer copy
      m_rowBuffer =  coll->dataEditor().rowBuffer();
      m_dstRefName = coll->description().eventReferenceColumnName();
   }
   if( m_queue.size() >= (size_t)m_maxOpen ) {
      reduceQueue();
   }
   m_queue.push( coll );
}

void  CollectionPool::reduceQueue() {
   ICollection *coll = m_queue.front();
   m_queue.pop();
   writeCache( coll );
   coll->commit();
   coll->close();
}

void  CollectionPool::writeCache( pool::ICollection* coll ) {
   rowVect_t &rowVect = m_rowCache[ coll ];
   for( rowVect_t::const_iterator ri = rowVect.begin(), rend = rowVect.end();
	ri != rend; ++ri ) {
      coll->dataEditor().insertRow( *ri );
   }
   rowVect.clear();
}





// NOTE - the current collection can NOT be deleted before calling ++
CollectionPool::flushing_iterator&
CollectionPool::flushing_iterator::operator++()
{
   ICollection *coll = m_iter->first;
   // reopen to trigger writing out of cached rows
   // NOTE - this is done >during< moving to the next collection
   if( !coll->isOpen() && m_iter->second.size() ) {
      coll->open();
   }
   if( coll->isOpen() ) {
      m_cpool->writeCache( coll );
      coll->commit();
      coll->close();
   }
   ++m_iter;
   return *this;
}


ICollection*	CollectionPool::flushing_iterator::operator->()
{
   ICollection *coll = m_iter->first;
   if( !coll->isOpen() && m_forceOpen ) {
      coll->open();
   }
   return coll;
}



