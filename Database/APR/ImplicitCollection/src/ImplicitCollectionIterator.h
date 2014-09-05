/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMPLICITCOLLECTION_COLLECTIONITERATOR_H
#define IMPLICITCOLLECTION_COLLECTIONITERATOR_H

//#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"

#include "CollectionBase/CollectionRowBuffer.h"

#include "PersistencySvc/IPositionSeek.h"
#include "AthenaKernel/ICollectionSize.h"

class Token;

namespace coral { class AttributeList; }

namespace pool {

   // forward declarations
   class IContainer;
   class ITokenIterator;
   class TokenList;
   class ICollectionDescription;
   
   /** 
    * @class ImplicitCollectionIterator ImplicitCollectionIterator.h Implicitcollection/ImplicitCollectionIterator.h
    *
    * Iterator over an implicit collection.
    * Single class implementing both Query and Cursor interfaces
    * to simplify backward compatibility
    */
   class ImplicitCollectionIterator : public ICollectionQuery,
                                      public ICollectionCursor,
                                      virtual public IPositionSeek,
                                      virtual public ICollectionSize
   {
  public:
     /// Constructor
     ImplicitCollectionIterator(IContainer& container,
                                const pool::ICollectionDescription& description );

     // ------------------- Query interface
     
     /**
      * Sets the query.
      * @param whereClause The query.
      * @param attributeBindData  - unused
      * @param tokenBindData  - unused
      */
     virtual void setCondition( const std::string& whereClause,
                                coral::AttributeList* attributeBindData = 0,
                                TokenList* tokenBindData = 0 ) ;

     /// Returns the where clause of the query.
     virtual const std::string& whereClause() const;

     /// Processes the query and returns a cursor over the query result.
     /// this method returns self
     virtual pool::ICollectionCursor& execute();


     // ------------------- Cursor interface 

     /** Retrieves the next token in the collection.
         Returns false if the end of the collection has been reached.
     */
     virtual bool                         next();

     /// Returns the token of the current position
     virtual Token*                         token() const;
     virtual const Token&                 eventRef() const { return *token(); }

     virtual const CollectionRowBuffer& currentRow() const;

     // ------------------- Seek interface 
     /**
      * @brief Seek to a given position in the collection
      * @param position  The position to which to seek.
      * @returns True if successful, false otherwise.
      */
     virtual bool seek (long long int position);

     /**
      * @brief Return the size of the collection.
      */
     virtual int size ();

     
     //------------------------------------------

     /// Destructor
     virtual ~ImplicitCollectionIterator();


     // ------------------- Unimplemented methods

     virtual void addToOutputList( const std::string& ) {}
     virtual void addToOutputList( const std::vector<std::string>& ) {}
     virtual void selectAllAttributes() {}
     virtual void selectAllTokens() {}
     virtual void selectAll() {}
     virtual void addToCollectionFragmentList( const std::string& ) {}
     virtual void addToCollectionFragmentList( const std::vector< std::string >& ) {}
     virtual void addAllCollectionFragments() {}
     virtual void setRowCacheSize( int ) {}

     virtual void close() {}


  protected:
     IContainer&        m_container;
     ITokenIterator*    m_tokenIterator;
     Token*             m_token;

     mutable CollectionRowBuffer        m_rowBuffer;

     std::string                m_whereClause;
   };

}

#endif


