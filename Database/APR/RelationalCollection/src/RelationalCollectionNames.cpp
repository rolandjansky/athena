/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollectionNames.h"
#include <sstream>

std::string
pool::RelationalCollection::RelationalCollectionNames::collectionTypeName()
{
  return "RelationalCollection";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionHeadersTable()
{
  return "POOL_COLLECTIONS";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable()
{
  return "COLLECTION_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::dataTableNameVariableInCollectionHeadersTable()
{
  return "DATA_TABLE_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::linksTableNameVariableInCollectionHeadersTable()
{
  return "LINKS_TABLE_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::writtenRecordsVariableInCollectionHeadersTable()
{
  return "RECORDS_WRITTEN";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::deletedRecordsVariableInCollectionHeadersTable()
{
  return "RECORDS_DELETED";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable()
{
  return "CHILD_COLLECTION_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::foreignKeyNameVariableInCollectionHeadersTable()
{
  return "FOREIGN_KEY_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionDescriptionsTable()
{
  return "POOL_COLLECTIONS_DESC";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable()
{
  return "COLLECTION_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable()
{
  return "VARIABLE_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionVariableTypeVariableInCollectionDescriptionsTable()
{
  return "VARIABLE_TYPE";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionVariableMaxSizeVariableInCollectionDescriptionsTable()
{
  return "VARIABLE_MAXIMUM_SIZE";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionVariableSizeIsFixedVariableInCollectionDescriptionsTable()
{
  return "VARIABLE_SIZE_IS_FIXED";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionVariablePositionVariableInCollectionDescriptionsTable()
{
  return "VARIABLE_POSITION";
}



std::string
pool::RelationalCollection::RelationalCollectionNames::collectionVariableAnnotationVariableInCollectionDescriptionsTable()
{
  return "VARIABLE_ANNOTATION";
}

std::string
pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionDataTable( int iteration )
{
  if ( iteration == 0 ) return "POOL_COLLECTION_DATA";
  std::ostringstream os;
  os << "POOL_COLLECTION_DATA_" << iteration;
  return os.str();
}


std::string
pool::RelationalCollection::RelationalCollectionNames::recordIdVariableInCollectionDataTable()
{
  return "ID";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::oid_1_variableInCollectionDataTable()
{
  return "OID_1";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::oid_2_variableInCollectionDataTable()
{
  return "OID_2";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::variableDataVariableInCollectionDataTable( int position )
{
  std::ostringstream os;
  os << "VAR_" << position;
  return os.str();
}


std::string
pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionLinksTable( int iteration )
{
  if ( iteration == 0 ) return "POOL_COLLECTION_LINKS";
  std::ostringstream os;
  os << "POOL_COLLECTION_LINKS_" << iteration;
  return os.str();
}


std::string
pool::RelationalCollection::RelationalCollectionNames::linkIdVariableInCollectionLinksTable()
{
  return "LINK_ID";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::databaseIdVariableInCollectionLinksTable()
{
  return "DATABASE_ID";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::containerIdVariableInCollectionLinksTable()
{
  return "CONTAINER_ID";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::classIdVariableInCollectionLinksTable()
{
  return "CLASS_ID";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::technologyIdVariableInCollectionLinksTable()
{
  return "TECHNOLOGY_ID";
}



// The name of the table holding the collection index and unique constraint descriptions.
//MN:  added this implementation, guess
std::string
pool::RelationalCollection::RelationalCollectionNames::
nameOfCollectionIndexDescriptionsTable()
{
   return "POOL_COLL_INDX_DESC";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable()
{
  return "INDEX_NAME";
} 

std::string
pool::RelationalCollection::RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable()
{
  return "UNIQUE_CONSTRAINT_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionVariableNameVariableInCollectionIndexDescriptionsTable()
{
  return "VARIABLE_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable()
{
  return "COLLECTION_NAME";
}



std::string
pool::RelationalCollection::RelationalCollectionNames::metadataTable()
{
  return "POOL_COLL_METADATA";
}



std::string
pool::RelationalCollection::RelationalCollectionNames::collectionNameColumn()
{
  return "COLLECTION_NAME";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::metadataKeyColumn()
{
  return "MD_KEY";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::metadataValueColumn()
{
  return "MD_VALUE";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::nameOfCollectionLockTable()
{
  return "POOL_COLL_LOCK";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::entryIDVarInCollectionLockTable()
{
  return "ID";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::collectionNameVarInCollectionLockTable()
{
  return "COLLECTION";
}

std::string
pool::RelationalCollection::RelationalCollectionNames::clientInfoVarInCollectionLockTable()
{
  return "CLIENT_INFO";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::lockTypeVarInCollectionLockTable()
{
  return "LOCKTYPE";
}


std::string
pool::RelationalCollection::RelationalCollectionNames::timestampVarInCollectionLockTable()
{
  return "TIMESTAMP";
}
