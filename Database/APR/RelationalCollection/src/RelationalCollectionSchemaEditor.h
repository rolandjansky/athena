/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONSCHEMAEDITOR_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONSCHEMAEDITOR_H

#include "CollectionBase/ICollectionSchemaEditor.h"

#include <map>


namespace coral {
  class ISessionProxy;
  class AttributeList;
}

namespace pool {

  class ICollectionDescription;
  class CollectionDescription;
  class CollectionRowBuffer;

  namespace RelationalCollection {

    /** 
     * @class RelationalCollectionSchemaEditor RelationalCollectionSchemaEditor.h src/RelationalCollectionSchemaEditor.h
     *
     * An implementation of the ICollectionSchemaEditor interface used to define the schema of a collection.
     */
      class RelationalCollectionSchemaEditor : virtual public ICollectionSchemaEditor
    {
    public:
      /**
       * Constructor.
       *
       * @param session Reference to current database access session.
       * @param description Specification of collection properties.
       * @param dataTableNameForCollectionFragmentName Map of data table names using names of corresponding collection fragment names as keys.
       * @param linksTableNameForCollectionFragmentName Map of links table names using names of corresponding collection fragment names as keys.
       * @param tableTokenColumnPrefixForCollectionTokenColumnName Map of table column prefixes for Tokens using names of corresponding collection columns as keys.
       * @param tableAttributeColumnNameForCollectionAttributeColumnName Map of table column names for Attributes using names of corresponding collection columns as keys.
       * @param tableColumnNameForCollectionColumnName Map of table column names or prefixes using names of corresponding collection columns as keys.
       * @param collectionColumnNameForTableColumnPosition Map of collection column names using positions of corresponding table columns as keys.
       * @param tableAttributeList List of all Attributes defined by collection but using table column names.
       * @param collectionRowBuffer Collection row buffer containing all Tokens and Attributes defined by collection.
       * @param dataTableRowBufferForCollectionFragmentName Map of data table row buffers using names of corresponding collection fragments as keys.
       * @param mapOfWhereClausesForOID1InDataTable Map of where clauses for finding Token OID_1 values in data tables, using Token names as keys.
       * @param mapOfWhereDataForOID1InDataTable Map of where data for finding Token OID_1 values in data tables, using Token names as keys.
       */
      RelationalCollectionSchemaEditor( 
                coral::ISessionProxy& session, 
                pool::ICollectionDescription& description,
                std::map< std::string, std::string >& dataTableNameForCollectionFragmentName,
                std::map< std::string, std::string >& linksTableNameForCollectionFragmentName, 
                std::map< std::string, std::string >& tableTokenColumnPrefixForCollectionTokenColumnName,
                std::map< std::string, std::string >& tableAttributeColumnNameForCollectionAttributeColumnName,
                std::map< std::string, std::string >& tablColumnNameForCollectionColumnName,
                std::map< int, std::string >& collectionColumnNameForTableColumnPosition,
                coral::AttributeList& tableAttributeList,
                pool::CollectionRowBuffer& collectionRowBuffer,
                std::map< std::string, coral::AttributeList* >& dataTableRowBufferForCollectionFragmentName,
                std::map< std::string, std::string >& mapOfWhereClausesForOID1InDataTable,
                std::map< std::string, coral::AttributeList* >& mapOfWhereDataForOID1InDataTable );

      /// Default constructor.
      ~RelationalCollectionSchemaEditor();

      RelationalCollectionSchemaEditor (const RelationalCollectionSchemaEditor&) = delete;
      RelationalCollectionSchemaEditor& operator= (const RelationalCollectionSchemaEditor&) = delete;

      /**
       * Sets the name of the event reference Token column. Otherwise a default name is used.
       *
       * @param columnName Name of event reference Token column.
       */
      virtual void setEventReferenceColumnName( const std::string& columnName );

      /**
       * Adds a new column to the collection fragment specified as input. If no collection 
       * fragment is specified the column is added to the top level collection fragment. 
       * Throws an exception if the specified collection fragment has not been declared 
       * to be part of the collection via a call to the method `addCollectionFragment'.
       *
       * @param columnName Name of new column.
       * @param columnType Data type of new column.
       * @param fragmentName Name of collection fragment to contain new column.
       * @param maxSize Maximum size of column data type (useful for string or blob data types).
       * @param sizeIsFixed Flag indicating whether size of column data type is fixed (useful for string or blob data types).
       */
      virtual const ICollectionColumn&    insertColumn( const std::string& columnName, 
                                 const std::string& columnType,
                                 const std::string& annotation = "",        
                                 std::string fragmentName = "",
                                 int maxSize = 0,
                                 bool sizeIsFixed = true );

      /**
       * Adds a new column to the collection fragment specified as input. If no collection 
       * fragment is specified the column is added to the top level collection fragment. 
       * Throws an exception if the specified collection fragment has not been declared to be 
       * part of the collection via a call to the method `addCollectionFragment'.
       * 
       * @param columnName Name of new column.
       * @param columnType Data type of new column.
       * @param fragmentName Name of collection fragment to contain new column.
       * @param maxSize Maximum size of column data type (useful for string or blob data types).
       * @param sizeIsFixed Flag indicating whether size of column data type is fixed (useful for string or blob data types).
       */
      virtual const ICollectionColumn&    insertColumn( const std::string& columnName, 
                                 const std::type_info& columnType,
                                 const std::string& annotation = "",
                                 std::string fragmentName = "",
                                 int maxSize = 0,
                                 bool sizeIsFixed = true );

      /**
       * Adds a new column of type pool::Token to the collection fragment specified as input. 
       * If no collection fragment is specified the column is added to the top level collection 
       * fragment. Throws an exception if the specified collection fragment has not been declared to be 
       * part of the collection via a call to the method `addCollectionFragment'. Throws an exception if 
       * an attempt is made to add the event reference Token column to any other collection fragment 
       * than the top level fragment.
       *
       * @param columnName Name of new column.
       * @param fragmentName Name of collection fragment to contain new column.
       */
      virtual const ICollectionColumn&    insertTokenColumn( const std::string& columnName,
                                      const std::string& annotation = "",
                                      std::string fragmentName = "" );


      /// add annotation to column
      virtual  const ICollectionColumn&    annotateColumn( const std::string& columnName,
                                                           const std::string& annotation ); 



      // Add row for new column into collection descriptions table.
      virtual void addRowToDescriptionsTable(const std::string& fragmentName,
                                             const std::string& columnName,
                                             const std::string& annotation,
                                             const std::string& colType, unsigned maxSize,
                                             const std::string& isFixed, unsigned newPos );
      
      // update all the maps (in RelationalCollection) when adding a new token column
      virtual void addTokenColumnToMaps( std::string variableName,
                                         const std::string& columnName, int columnPos );
      
      virtual void addTokenColumnToRowBuffer( coral::AttributeList* rowBuffer,
                                              const std::string& tableColumnPrefix );
      
      virtual void addColumnToMaps( const std::string& columnName,
                                    const std::string& tableColumnName,
                                    const std::string& columntype, int columnPos );
      
      virtual void recreateDataTableRowBuffer( const std::string& fragmentName );

      /**
       * Removes a column from the collection.
       *
       * @param columnName Name of column to be removed.
       */
      virtual void dropColumn( const std::string& columnName );

      /**
       * Renames a column of the collection.
       *
       * @param oldName Old name of column.
       * @param newName New name of column.
       */
      virtual void renameColumn( const std::string& oldName, const std::string& newName );

      /**
       * Changes the data type of a column in the collection. Throws an exception if an attempt is 
       * made to change the data type of the event reference Token column.
       * 
       * @param columnName Name of column whose type is to be changed.
       * @param newType New data type assigned to column.
       * @param maxSize Maximum size of new data type (useful for string and blob types).
       * @param sizeIsFixed Flag indicating whether size of new data type is fixed (useful for string and blob types).
       */
      virtual void changeColumnType( const std::string& columnName,
                                     const std::string& newType,
                                     int maxSize = 0,
                                     bool sizeIsFixed = true );

      /**
       * Changes the data type of a column in the collection. Throws an exception if an attempt is 
       * made to change the data type of the event reference Token column.
       * 
       * @param columnName Name of column whose type is to be changed.
       * @param newType New data type assigned to column.
       * @param maxSize Maximum size of new data type (useful for string and blob types).
       * @param sizeIsFixed Flag indicating whether size of new data type is fixed (useful for string and blob types).
       */
      virtual void changeColumnType( const std::string& columnName,
                                     const std::type_info& newType,
                                     int maxSize = 0,
                                     bool sizeIsFixed = true );

      /**
       * Creates an index on a column of the collection. Automatically generates a unique name for the index.
       * 
       * @param columnName Name of column for which index is created.
       * @param isUnique Flag to indicate whether indexed column values must be unique.
       */
      virtual void createIndex( std::string indexName,
                                const std::string& columnName, 
                                bool isUnique = false );

      /**
       * Creates an index on one or more columns of the collection. Automatically generates a unique name
       * for the index
       *
       * @param columnNames Names of columns for which index is to be created.
       * @param isUnique Flag to indicates whether combination of indexed column values must be unique.
       */
      virtual void createIndex( std::string indexName,
                                const std::vector< std::string >& columnNames, 
                                bool isUnique = false );

      /**
       * Removes an index from the collection, given the name of the column on
       * which the index to be removed is applied.
       *
       * @param columnName Name of column on which index to be removed is applied.
       */
      virtual void dropIndex( const std::string& columnName );

      /**
       * Removes an index from a column of the collection, given the names of the columns on
       * which the index to be removed is applied.
       *
       * @param columnNames Names of columns on which index to be removed is applied.
       */
      virtual void dropIndex( const std::vector<std::string>& columnNames );

      /**
       * Sets a unique constraint on a column of the collection. Automatically generates a unique name
       * for the unique constraint.
       *
       * @param columnName Name of column for which constraint is applied.
       */
      void setUniqueConstraint( const std::string& constraintName, const std::string& columnName );

      /**
       * Sets a unique constraint on one or more columns of the collection. Automatically generates a
       * unique name for the unique constraint.
       *
       * @param columnNames Names of columns for which constraint is applied.
       */
      void setUniqueConstraint( const std::string& constraintName, const std::vector<std::string>& columnNames );

      /**
       * Unsets an existing unique constraint on a single column of the collection, 
       * given the name of the column.
       *
       * @param columnName Name of column used by unique constraint to be unset.
       */
      void unsetUniqueConstraint( const std::string& columnName );

      /**
       * Unsets an existing unique constraint on one or more columns of the colleciton, 
       * given the names of the columns.
       *
       * @param columnNames Names of columns used by unique constraint to be unset.
       */
      void unsetUniqueConstraint( const std::vector<std::string>& columnNames );

      /**
       * Adds a collection fragment to the collection. If no parent collection fragment name is provided
       * as input the last collection fragment in the fragment chain is assigned as the parent collection fragment of
       * the new collection fragment.
       *
       * @param fragmentName Name of collection fragment to add to collection.
       * @param parentFragmentName Name of collection fragment to assign as parent of new collection fragment.
       * @param usesForeignKey Flag indicating whether new fragment is to reference its parent fragment via a foreign key.
       */
      virtual void addCollectionFragment( const std::string& fragmentName,
                                          std::string parentFragmentName = "",
                                          bool usesForeignKey = true );

      /**
       * Drops a collection fragment from the collection. After the fragment is dropped its parent
       * collection fragment is assigned as the new parent of its child collection fragment.
       * 
       * @param fragmentName Name of collection fragment to drop from collection description.
       */
      virtual void dropCollectionFragment( const std::string& fragmentName );

      /**
       * Renames a collection fragment in the collection. Regenerates unique names for all indices,
       * unique constraints and foreign key constraints defined for the fragment.
       * 
       * @param oldName Old name of collection fragment.
       * @param newName New name of collection fragment.
       */
      virtual void renameCollectionFragment( const std::string& oldName, const std::string& newName );

      virtual const std::string& dataTableNameForCollectionFragmentName( const std::string& fragmentName, const std::string& methodName ) const;
      
   protected:

      // refresh the attribute lists after adding or removing collection fragments
      void        rebuildAttributeLists();
      
   private:
      /// Reference to current database access session.
      coral::ISessionProxy        *m_session;

      /// Specification of collection properties.
      CollectionDescription        *m_description;

      /// Map of data table names using names of corresponding collection fragment names as keys.
      std::map< std::string, std::string >& m_dataTableNameForCollectionFragmentName;

      /// Map of links table names using names of corresponding collection fragment names as keys.
      std::map< std::string, std::string >& m_linksTableNameForCollectionFragmentName;

      /// Map of table column prefixes for Tokens using names of corresponding collection columns as keys.
      std::map< std::string, std::string >& m_tableTokenColumnPrefixForCollectionTokenColumnName;

      /// Map of table column names for Attributes using names of corresponding collection columns as keys.
      std::map< std::string, std::string >& m_tableAttributeColumnNameForCollectionAttributeColumnName;

      /// Map of table column names or prefixes using names of corresponding collection columns as keys.
      std::map< std::string, std::string >& m_tableColumnNameForCollectionColumnName;

      /// Map of collection column names using positions of corresponding table columns as keys.
      std::map< int, std::string >& m_collectionColumnNameForTableColumnPosition;

      /// List of all Attributes defined by collection but using table column names.
      coral::AttributeList& m_tableAttributeList;
  
      /// Collection row buffer containing all Tokens and Attributes defined by collection.
      CollectionRowBuffer& m_collectionRowBuffer;
  
      /// Map of data table row buffers using names of corresponding collection fragments as keys.
      std::map< std::string, coral::AttributeList* >& m_dataTableRowBufferForCollectionFragmentName;

      /// Map of where clauses for finding Token OID_1 values in data tables, using Token names as keys.
      std::map< std::string, std::string >& m_mapOfWhereClausesForOID1InDataTable;

      /// Map of where data for finding Token OID_1 values in data tables, using Token names as keys.
      std::map< std::string, coral::AttributeList* >& m_mapOfWhereDataForOID1InDataTable;

      /// Where clause for finding collection fragment name in collection headers table.
      std::string m_whereClauseForCollectionNameInHeadersTable;

      /// Where clause for finding child collection fragment name in collection headers table.
      std::string m_whereClauseForChildCollectionNameInHeadersTable;

      /// Where clause for finding collection fragment name in collection descriptions table.
      std::string m_whereClauseForCollectionNameInDescriptionsTable;

      /// Where clause for finding column name in collection descriptions table.
      std::string m_whereClauseForColumnNameInDescriptionsTable;

      /// Where clause for finding collection fragment name in index and unique constraints descriptions table.
      std::string m_whereClauseForCollectionNameInIndexDescriptionsTable;

      // Where clause for finding index name in index and unique constraint descriptions table.
      std::string m_whereClauseForIndexNameInIndexDescriptionsTable;

      // Where clause for finding unique constraint name in index and unique constraint descriptions table.
      std::string m_whereClauseForUniqueConstraintNameInIndexDescriptionsTable;

      /// Bind data for collection headers table collection fragment name where clause.
      coral::AttributeList* m_whereDataForCollectionNameInHeadersTable;

      /// Bind data for collection headers table child collection fragment name where clause.
      coral::AttributeList* m_whereDataForChildCollectionNameInHeadersTable;

      /// Bind data for collection descriptions table collection fragment name where clause.
      coral::AttributeList* m_whereDataForCollectionNameInDescriptionsTable;

      /// Bind data for collection descriptions table column name where clause.
      coral::AttributeList* m_whereDataForColumnNameInDescriptionsTable;

      /// Bind data for collection index descriptions table collection fragment name where clause.
      coral::AttributeList* m_whereDataForCollectionNameInIndexDescriptionsTable;

      /// Bind data for collection index descriptions table index name where clause.
      coral::AttributeList* m_whereDataForIndexNameInIndexDescriptionsTable;

      /// Bind data for collection index descriptions table unique constraint name where clause.
      coral::AttributeList* m_whereDataForUniqueConstraintNameInIndexDescriptionsTable;
    };
  }
}

#endif

