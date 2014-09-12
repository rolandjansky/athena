/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONCURSOR_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONCURSOR_H

#include "CollectionBase/ICollectionCursor.h" 

#include <string>
#include <map>


namespace coral {
  class ICursor;
}

namespace pool {

  class ICollectionDescription;

  namespace RelationalCollection {

    /** 
     * @class RelationalCollectionCursor RelationalCollectionCursor.h src/RelationalCollectionCursor.h
     *
     * Implementation of the ICollectionCursor interface used to navigate the result of a query on a collection.
     */
    class RelationalCollectionCursor : virtual public pool::ICollectionCursor
    {
    public:
      /**
       * Constructor.
       *
       * @param cursor Cursor for navigation over rows that pass query.
       * @param description Specification of collection properties.
       * @param mapOfTokenKeyForLinkIdMaps Map of maps of Token prefixes that use corresponding link table ID's as keys, using collection fragment names as keys.
       * @param collectionRowBuffer Row buffer containing Tokens and Attributes selected by query.
       */
      RelationalCollectionCursor( 
         coral::ICursor& cursor,
           const ICollectionDescription& description,
           const std::map< std::string, std::map< unsigned, std::string >* >& mapOfTokenKeyForLinkIdMaps,
           const CollectionRowBuffer& collectionRowBuffer );

      /// Destructor.
      ~RelationalCollectionCursor();

      RelationalCollectionCursor (const RelationalCollectionCursor&) = delete;
      RelationalCollectionCursor& operator= (const RelationalCollectionCursor&) = delete;

      /// Advances the cursor to the next row of the query result set.
      virtual bool next();

      /// Returns the selected Tokens and Attributes for the current row of the query result set.
      virtual const pool::CollectionRowBuffer& currentRow() const;

      /// Returns the event reference Token for the current row.
      virtual const Token& eventRef() const;

      /// Returns the primary key for the current row.
      virtual unsigned primaryKey() const;

      /// Cleanup.
      virtual void close();

    private:
      /// Cursor for navigation over rows that pass query.
      coral::ICursor&         m_cursor;

      /// Specification of collection properities.
      const ICollectionDescription& m_description;

      /// Map of maps of Token prefixes that use corresponding link table ID's as keys, using collection fragment names as keys.
      const std::map< std::string, std::map< unsigned, std::string >* >& m_mapOfTokenKeyForLinkIdMaps;

      /// Row buffer containing Tokens and Attributes selected by query.
      pool::CollectionRowBuffer        *m_collectionRowBuffer;

      /// Primary key for current row.
      unsigned                        m_primaryKey;
    };
  }
}

#endif


