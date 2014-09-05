/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONNAMES_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONNAMES_H

#include <string>

namespace pool {

  namespace RelationalCollection {

    /// Utility class holding the string literals used in the RelationalCollection package.
    class RelationalCollectionNames {
    public:
      /// The type name of these collections.
      static std::string collectionTypeName();

      // The collection headers table.

      /// The name of the table holding the collection headers.
      static std::string nameOfCollectionHeadersTable();
      /// The variable name holding the collection name in the table holding the collection headers.
      static std::string collectionNameVariableInCollectionHeadersTable();
      /// The variable name holding the data table name in the table holding the collection headers.
      static std::string dataTableNameVariableInCollectionHeadersTable();
      /// The variable name holding the links table name in the table holding the collection headers.
      static std::string linksTableNameVariableInCollectionHeadersTable();
      /// The variable name holding the number of records written in the table holding the collection headers.
      static std::string writtenRecordsVariableInCollectionHeadersTable();
      /// The variable name holding the number of records deleted in the table holding the collection headers.
      static std::string deletedRecordsVariableInCollectionHeadersTable();
      /// The variable name holding the child collection fragment name in the table holding the collection headers.
      static std::string childCollectionNameVariableInCollectionHeadersTable();
      /// The variable name holding the foreign key name in the table holding the collection headers.
      static std::string foreignKeyNameVariableInCollectionHeadersTable();

      // The collection descriptions table.

      /// The name of the table holding the collection descriptions.
      static std::string nameOfCollectionDescriptionsTable();
      /// The variable name holding the collection name in the table holding the collection descriptions.
      static std::string collectionNameVariableInCollectionDescriptionsTable();
      /// The variable name holding the collection variable name in the table holding the collection descriptions.
      static std::string collectionVariableNameVariableInCollectionDescriptionsTable();
      /// The variable name holding the collection variable type in the table holding the collection descriptions.
      static std::string collectionVariableTypeVariableInCollectionDescriptionsTable();
      /// The variable name holding the collection variable type's maximum size in the table holding the collection descriptions.
      static std::string collectionVariableMaxSizeVariableInCollectionDescriptionsTable();
      /// The variable name holding the collection variable's fixed size flag in the table holding the collection descriptions.
      static std::string collectionVariableSizeIsFixedVariableInCollectionDescriptionsTable();
      /// The variable name holding the position of a collection variable in the table holding the collection descriptions.
      static std::string collectionVariablePositionVariableInCollectionDescriptionsTable();

      /// Variable name holding collection attribute annotations in the collection descriptions
      static std::string collectionVariableAnnotationVariableInCollectionDescriptionsTable();

      // The collection data table.

      /// The name of the table holding the collection data.
      static std::string nameOfCollectionDataTable( int iteration );
      /// The record id variable name in the table holding the collection data.
      static std::string recordIdVariableInCollectionDataTable();
      /// The variable name holding the first part of the OID in the table holding the collection data.
      static std::string oid_1_variableInCollectionDataTable();
      /// The variable name holding the second part of the OID in the table holding the collection data.
      static std::string oid_2_variableInCollectionDataTable();
      /// The variable name the data of a variable in the table holding the collection data.
      static std::string variableDataVariableInCollectionDataTable( int position );

      // The collection links table.

      /// The name of the table holding the collection links.
      static std::string nameOfCollectionLinksTable( int iteration );
      /// The link ID variable in the table holding the collection links.
      static std::string linkIdVariableInCollectionLinksTable();
      /// The database ID variable in the table holding the collection links.
      static std::string databaseIdVariableInCollectionLinksTable();
      /// The container ID variable in the table holding the collection links.
      static std::string containerIdVariableInCollectionLinksTable();
      /// The class ID variable in the table holding the collection links.
      static std::string classIdVariableInCollectionLinksTable();
      /// The technology ID variable in the table holding the collection links.
      static std::string technologyIdVariableInCollectionLinksTable();

      // The collection index and unique constraint descriptions table.

      /// The name of the table holding the collection index and unique constraint descriptions.
      static std::string nameOfCollectionIndexDescriptionsTable();
      /// The variable name holding the index name in the table holding the collection index descriptions.
      static std::string indexNameVariableInCollectionIndexDescriptionsTable();
      /// The variable name holding the collection unique constraint name in the table holding the collection index descriptions.
      static std::string uniqueConstraintNameVariableInCollectionIndexDescriptionsTable();
      /// The variable name holding the collection variable name in the table holding the collection index descriptions.
      static std::string collectionVariableNameVariableInCollectionIndexDescriptionsTable();
      /// The variable name holding the collection name in the table holding the collection index descriptions.
      static std::string collectionNameVariableInCollectionIndexDescriptionsTable();


      // metadata table
      static std::string metadataTable();
      static std::string collectionNameColumn();
      static std::string metadataKeyColumn();
      static std::string metadataValueColumn();
 
      // LOCK table
      static std::string nameOfCollectionLockTable();
      static std::string entryIDVarInCollectionLockTable();
      static std::string collectionNameVarInCollectionLockTable();
      static std::string clientInfoVarInCollectionLockTable();
      static std::string lockTypeVarInCollectionLockTable();
      static std::string timestampVarInCollectionLockTable();


    };
  }
}

#endif
