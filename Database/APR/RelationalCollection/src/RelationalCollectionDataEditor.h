/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONDATAEDITOR_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONDATAEDITOR_H

#include "CollectionBase/ICollectionDataEditor.h"

#include "RelationalCollectionFragmentData.h"


namespace coral {
   class ISessionProxy;
   class IBulkOperation;
   class ISchema;
}

namespace pool {

  class ICollectionDescription;

  namespace RelationalCollection {

    /** 
     * @class RelationalCollectionDataEditor RelationalCollectionDataEditor.h src/RelationalCollectionDataEditor.h
     *
     * Implementation of the ICollectionDataEditor interface used to add, update or delete data in a collection.
     */
     class RelationalCollectionDataEditor : virtual public pool::ICollectionDataEditor
     {

    friend class RelationalCollection;
    public:
      /**
       * Constructor.

       * @param session Reference to current database access session.
       * @param description Specification of collection properties.
       * @param dataTableNameForCollectionFragmentName Map of data table names using names of corresponding collection fragment names as keys.
       * @param linksTableNameForCollectionFragmentName Map of links table names using names of corresponding collection fragment names as keys.
       * @param tableTokenColumnPrefixForCollectionTokenColumnName Map of table column prefixes for Tokens using names of corresponding collection columns as keys.
       * @param tableAttributeColumnNameForCollectionAttributeColumnName Map of table column names for Attributes using names of corresponding collection columns as keys.
       * @param tableAttributeList List of all Attributes defined by collection but using table column names.
       * @param collectionRowBuffer Collection row buffer containing all Tokens and Attributes defined by collection.
       * @param dataTableRowBufferForCollectionFragmentName Map of data table row buffers using names of corresponding collection fragments as keys.
       * @param linksTableRowBuffer Row buffer using schema of collection links tables.
       * @param mapOfLinkIdForTokenKeyMaps Map of maps of link table ID's that use corresponding Token prefixes as keys, using collection fragment names as keys.
       * @param mapOfTokenKeyForLinkIdMaps Map of maps of Token prefixes that use corresponding link table ID's as keys, using collection fragment names as keys.
       * @param mapOfWhereClausesForOID1InDataTable Map of where clauses for finding Token OID_1 values in data tables, using Token names as keys.
       * @param mapOfWhereDataForOID1InDataTable Map of where data for finding Token OID_1 values in data tables, using Token names as keys.
       */
      RelationalCollectionDataEditor(
        coral::ISessionProxy& session,
        const pool::ICollectionDescription& description,
        const std::map< std::string, std::string >& dataTableNameForCollectionFragmentName,
        const std::map< std::string, std::string >& linksTableNameForCollectionFragmentName,
        FragmentDataMap& fragmentDataMap,
        const std::map< std::string, std::string >& tableTokenColumnPrefixForCollectionTokenColumnName,
        const std::map< std::string, std::string >& tableAttributeColumnNameForCollectionAttributeColumnName,
        const coral::AttributeList& tableAttributeList,
        CollectionRowBuffer& collectionRowBuffer,
        std::map< std::string, coral::AttributeList* >& dataTableRowBufferForCollectionFragmentName,
        coral::AttributeList& linksTableRowBuffer,
        std::map< std::string, std::map< std::string, unsigned >* >& mapOfLinkIdForTokenKeyMaps,
        std::map< std::string, std::map< unsigned, std::string >* >& mapOfTokenKeyForLinkIdMaps,
        const std::map< std::string, std::string >& mapOfWhereClausesForOID1InDataTable,
        const std::map< std::string, coral::AttributeList* >& mapOfWhereDataForOID1InDataTable );

      /**
       * Destructor.
       */
      ~RelationalCollectionDataEditor();

      RelationalCollectionDataEditor (const RelationalCollectionDataEditor&) = delete;
      RelationalCollectionDataEditor& operator= (const RelationalCollectionDataEditor&) = delete;

      /**
       * Sets the number of rows to cache before insertion into the collection. If the input
       * argument is 0 (the default) bulk row insertion will not be applied.
       *
       * @param rowCacheSize Number of rows to cache before insertion into collection.
       */
      virtual void setRowCacheSize( int rowCacheSize = 0 );

      /// Allocate "step" row primary keys every time more is needed
      void        setRowIDAllocationStep( int step );

      /**
       * Returns a reference to a collection row buffer for adding rows of data to the collection.
       * If data for one or more columns will not be provided use the method `emptyRowBuffer' instead.
       */
      virtual CollectionRowBuffer& rowBuffer();

      /**
       * Returns a reference to an empty collection row buffer for adding rows of data to the collection.
       * If data will be provided for all columns use the method `rowBuffer' instead since buffers do
       * not need to be cleared in this case.
       */
      virtual CollectionRowBuffer& emptyRowBuffer();

      /**
       * Clears all row buffers. Should be applied before inserting rows for which some columns are
       * expected to have null values.
       */
      virtual void clearRowBuffers();

      /**
       * Adds a new row of data to the collection. An option is provided to turn off the
       * written records counter to improve performance.
       *
       * @param inputRowBuffer Buffer containing data for row to be added.
       * @param updateRecordsCounter Flag indicating whether to increment written records counter. 
       */
      virtual void insertRow( const pool::CollectionRowBuffer& inputRowBuffer, bool updateRecordsCounter = true );

      /**
       * Updates the rows of the collection that satisfy a query. Returns the number of rows updated.
       * Throws an exception if both the Attribute and Token set lists provided as input are empty.
       *
       * @param tokenSetList List of Tokens to set with associated values.
       * @param attributeSetList List of Attributes to set with associated values.
       * @param whereClause Predicates of query. 
       * @param attributeBindData Attribute bind data for where clause.
       * @param tokenBindData Token bind data for where clause.
       */
      virtual int updateRows( coral::AttributeList* attributeSetList = 0,
                               TokenList* tokenSetList = 0,
                               const std::string& whereClause = "",
                               coral::AttributeList* attributeBindData = 0,
                               pool::TokenList* tokenBindData = 0 );

      /**
       * Deletes the rows of the collection that satisfy a query. Returns the number of rows deleted.
       * Throws an exception if no where clause is provided as input. An option is provided to turn
       * off the written and deleted records counters if the written records counter was turned off
       * during record insertion to improve performance.
       *
       * @param whereClause Predicates of query.
       * @param attributeBindData Attribute bind data for where clause.
       * @param tokenBindData Token bind data for where clause.
       * @param updateRecordsCounter Flag indicating whether to decrement written records counter and increment deleted records counter.
       */
      virtual int deleteRows( const std::string& whereClause,
                               coral::AttributeList* attributeBindData = 0,
                               pool::TokenList* tokenBindData = 0,
                               bool updateRecordsCounter = true );

    protected:
       /// Create Coral bulk insert structures
       virtual void     createRowCache();
       
       /// Flush all cached rows in case of bulk operations
       virtual void     flushCachedRows();
      
       /// Cleanup Coral structures used for bulk operations
       virtual void     deleteRowCache();

       /// Generate unique rowID to be used as PK in data and link tables
       virtual int      getUniqueRowID();
      
   private:
      /// If greater than 0, do bulk inserts grouping m_rowCacheSize rows
      int                                       m_rowCacheSize;

      /// Allocate m_rowIDAllocationStep primary keys every time
      int                                       m_rowIDAllocationStep;

      /// Reference to the current database access session.
      coral::ISessionProxy&                     m_session;

      /// Current database schema
      coral::ISchema*                           m_schema;

      /// Specification of collection properties.
      const ICollectionDescription&             m_description;

      /// Map of data table names using names of corresponding collection fragment names as keys.
      const std::map< std::string, std::string >& m_dataTableNameForCollectionFragmentName;

      /// Map of links table names using names of corresponding collection fragment names as keys.
      const std::map< std::string, std::string >& m_linksTableNameForCollectionFragmentName;

      /// Information related to fragments, map keyed by fragment name
      FragmentDataMap&                        m_fragmentDataMap;

      /// Map of table column prefixes for Tokens using names of corresponding collection columns as keys.
      const std::map< std::string, std::string >& m_tableTokenColumnPrefixForCollectionTokenColumnName;

      /// Map of table column names for Attributes using names of corresponding collection columns as keys.
      const std::map< std::string, std::string >& m_tableAttributeColumnNameForCollectionAttributeColumnName;

      /// List of all Attributes defined by collection but using table column names.
      const coral::AttributeList& m_tableAttributeList;
  
      /// Collection row buffer containing all Tokens and Attributes defined by collection.
      CollectionRowBuffer& m_collectionRowBuffer;

      /// Map of data table row buffers using names of corresponding collection fragments as keys.
      std::map< std::string, coral::AttributeList* >& m_dataTableRowBufferForCollectionFragmentName;

      /// Row buffer using schema of collection links tables.
      coral::AttributeList& m_linksTableRowBuffer;

      /// Map of maps of link table ID's that use corresponding Token prefixes as keys, using collection fragment names as keys.
      std::map< std::string, std::map< std::string, unsigned >* >& m_mapOfLinkIdForTokenKeyMaps;

      /// Map of maps of Token prefixes that use corresponding link table ID's as keys, using collection fragment names as keys.
      std::map< std::string, std::map< unsigned, std::string >* >& m_mapOfTokenKeyForLinkIdMaps;

      /// Map of where clauses for finding Token OID_1 values in data tables, using Token names as keys.
      const std::map< std::string, std::string >& m_mapOfWhereClausesForOID1InDataTable;

      /// Map of where data for finding Token OID_1 values in data tables, using Token names as keys.
      const std::map< std::string, coral::AttributeList* >& m_mapOfWhereDataForOID1InDataTable;

      /// Where clause for finding collection fragment name in collection headers table.
      std::string m_whereClauseForCollectionNameInHeadersTable;

      /// Bind data for collection headers table collection fragment name where clause.
      coral::AttributeList* m_whereDataForCollectionNameInHeadersTable;
  
      /// Where clause for finding primary key in collection data table.
      std::string m_whereClauseForPrimaryKeyInDataTable;

      /// Bind data for collection data table primary key where clause.
      coral::AttributeList* m_whereDataForPrimaryKeyInDataTable;
  
      /// Where clause for finding link ID in collection links table.
      std::string m_whereClauseForLinkIdInLinksTable;

      /// Bind data for collection links table link ID where clause.
      coral::AttributeList* m_whereDataForLinkIdInLinksTable;

      /// Map of bulk row insertion operation objects using names of corresponding collection fragments as keys.
      std::map< std::string, coral::IBulkOperation* > m_bulkRowOperationForCollectionFragmentName;

      // ----------------  variables for the rowID allocation session
      
      /// An secondary session with the same database, to allocate row IDs
      coral::ISessionProxy*                 m_rowIDSession;
  
    };
  }
}

#endif

