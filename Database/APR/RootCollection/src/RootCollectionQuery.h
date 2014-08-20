/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCOLLECTION_COLLECTIONQUERY_H
#define ROOTCOLLECTION_COLLECTIONQUERY_H

#include "CoralBase/AttributeList.h"

#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionCursor.h"

#include "RootCollectionCursor.h"

#include "TTree.h"

#include <set>

namespace pool {
   namespace RootCollection {

      /** 
       * @class RootCollectionQuery RootCollectionQuery.h Rootcollection/RootCollectionQuery.h
       *
       * An interface used to query a collection.
       */
      class RootCollectionQuery : public ICollectionQuery
      {
     public:
        /// Constructor
        RootCollectionQuery( const pool::ICollectionDescription& description, TTree *tree );
    
        /// Destructor
        virtual ~RootCollectionQuery();

        /**
         * Adds a column to the query select list.
         *
         * @param columnName Name of Attribute column to select.
         */
        virtual void addToOutputList( const std::string& columnName );

        /**
         * Adds one or more columns to the query select list.
         *
         * @param columnNames Names of Attribute columns to select.
         */
        virtual void addToOutputList( const std::vector<std::string>& columnNames );

        /// Adds all Attribute columns to the query select list.
        virtual void selectAllAttributes();

        /// Adds all Token columns to the query select list.
        virtual void selectAllTokens();

        /// Adds all Token and Attribute columns to the query select list.
        virtual void selectAll();

        /**
         * Adds the data table associated with a collection fragment to the query table list.
         *
         * @param fragmentName Name of collection fragment to add.
         */
        virtual void addToCollectionFragmentList( const std::string& fragmentName );

        /**
         * Adds the data tables associated with one or more collection fragments to the query 
         * table list.
         *
         * @param fragmentNames Names of collection fragments to add.
         */
        virtual void addToCollectionFragmentList( const std::vector< std::string >& fragmentNames );

        /// Adds all collection fragments to the query table list.
        virtual void addAllCollectionFragments();

        /**
         * Sets the query.
         * the query can be
         * constructed in fragments by repeated calls to `setCondition'. 
         *
         * @param whereClause The query.
         * @param attributeBindData  - unused
         * @param tokenBindData  - unused
         */
        virtual void setCondition( const std::string& whereClause,
                                   coral::AttributeList* attributeBindData = 0,
                                   TokenList* tokenBindData = 0 ) ;

        /// Returns the where clause of the query.
        virtual const std::string& whereClause() const;

        /**
         * Sets the cache size used to store the query result.
         *
         * @param Number of rows stored in cache before cache is flushed.
         */
        virtual void setRowCacheSize( int rowCacheSize );

        /// Processes the query and returns a cursor over the query result.
        virtual pool::ICollectionCursor& execute();

        /**
         * Tell the query to not include the primary event reference
         * in the result by default (it can still beselected manually)
         *
         * @param skip if true (the default) then skip the primary event reference
         */
        virtual void skipEventReference( bool = true );

     protected:

        void                addToTokenOutputList( const std::string& columnName );

        void                addToAttributeOutputList( const std::string& columnName );
        
        
        const ICollectionDescription    &m_description;

        TTree                           *m_tree;

        RootCollectionCursor            *m_cursor;

        std::string                     m_whereClause;

        pool::TokenList                 m_outputTokenList;
        coral::AttributeList            m_outputAttributeList;

        std::set< std::string >         m_selectedColumnNames;
        std::set< std::string >         m_collectionFragmentNames;

        /// If false, the primary event reference is added always to the query result
        bool                            m_skipEventRef; 
      };

   }
}

#endif


