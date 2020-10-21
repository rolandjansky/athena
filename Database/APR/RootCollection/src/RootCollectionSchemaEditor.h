/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCOLLECTION_COLLECTIONSCHEMAEDITOR_H
#define ROOTCOLLECTION_COLLECTIONSCHEMAEDITOR_H

#include <string>
#include <vector>
#include <typeinfo>

#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/ICollectionSchemaEditor.h"

#include "CoralBase/MessageStream.h"

#include "TTree.h"


namespace pool {
   class ICollectionColumn;

   namespace RootCollection {

      class RootCollection;
      
      /** 
       * @class RootCollectionSchemaEditor RootCollectionSchemaEditor.h CollectionBase/RootCollectionSchemaEditor.h
       *
       * An interface used to define the schema of a collection.
       */
      class RootCollectionSchemaEditor : public ICollectionSchemaEditor
      {
        
     friend class RootCollection;
        
     public:

        RootCollectionSchemaEditor( RootCollection& collection,
                                    CollectionDescription& description,
                                    TTree *tree  );

        
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

        virtual const ICollectionColumn&    insertColumn(
           const std::string& columnName, 
           const std::string& columnType,
           const std::string& annotation = "",
           std::string fragmentName = "",
           int maxSize = 0,
           bool sizeIsFixed = true );

        /**
         * Adds a new column to the collection fragment description object specified as input. If no collection 
         * fragment is specified the column is added to the description object of the top level collection fragment. 
         * Throws an exception if the specified collection fragment has not been declared to be part of the collection 
         * via a call to the method `addCollectionFragment'.
         * 
         * @param columnName Name of new column.
         * @param columnType Data type of new column.
         * @param fragmentName Name of collection fragment to contain new column.
         * @param maxSize Maximum size of column data type (useful for string or blob data types).
         * @param sizeIsFixed Flag indicating whether size of column data type is fixed (useful for string or blob data types).
         */
        virtual const ICollectionColumn&   insertColumn(
           const std::string& columnName, 
           const std::type_info& columnType,
           const std::string& annotation = "",
           std::string fragmentName = "",
           int maxSize = 0,
           bool sizeIsFixed = true );

        /**
         * Adds a new column of type pool::Token to the collection fragment description object specified as input. 
         * If no collection fragment is specified the column is added to the description object of the top level 
         * collection fragment. Throws an exception if the specified collection fragment has not been declared to be 
         * part of the collection via a call to the method `addCollectionFragment'. Throws an exception if an attempt 
         * is made to add the event reference Token column to any other collection fragment than the top level fragment.
         *
         * @param columnName Name of new column.
         * @param fragmentName Name of collection fragment to contain new column.
         */
        virtual const ICollectionColumn&    insertTokenColumn(
           const std::string& columnName,
           const std::string& annotation = "",
           std::string fragmentName = "" );

        
        /// add annotation to column
        virtual  const ICollectionColumn&    annotateColumn(
           const std::string& columnName,
           const std::string& annotation ); 
        
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
        virtual void setUniqueConstraint( const std::string& constraintName, const std::string& columnName );

        /**
         * Sets a unique constraint on one or more columns of the collection. Automatically generates a
         * unique name for the unique constraint.
         *
         * @param columnNames Names of columns for which constraint is applied.
         */
        virtual void setUniqueConstraint( const std::string& constraintName, const std::vector<std::string>& columnNames );

        /**
         * Unsets an existing unique constraint on a single column of the collection, 
         * given the name of the column.
         *
         * @param columnName Name of column used by unique constraint to be unset.
         */
        virtual void unsetUniqueConstraint( const std::string& columnName );

        /**
         * Unsets an existing unique constraint on one or more columns of the collection, 
         * given the names of the columns.
         *
         * @param columnNames Names of columns used by unique constraint to be unset.
         */
        virtual void unsetUniqueConstraint( const std::vector<std::string>& columnNames );

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
         * Renames a collection fragment in the collection description. Regenerates unique names for all indices,
         * unique constraints and foreign key constraints defined for the fragment.
         * 
         * @param oldName Old name of collection fragment.
         * @param newName New name of collection fragment.
         */
        virtual void renameCollectionFragment( const std::string& oldName, const std::string& newName );

        /// destructor.
        virtual ~RootCollectionSchemaEditor();

     protected:
        void         addTreeBranch( const std::string& name, const std::string& type_name );
        void        readSchema();
        void        createTreeBranches();
        void        writeSchema();
        
        RootCollection                        &m_collection;
        
        CollectionDescription                &m_description;

        TTree                                *m_tree;

        bool                                m_schemaWritten;

         mutable coral::MessageStream        m_poolOut; 
     };
   }
}
#endif

