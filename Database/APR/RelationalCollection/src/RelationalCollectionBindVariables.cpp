/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RelationalCollectionBindVariables.h"
#include "RelationalCollectionNames.h"

#include <sstream>


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForCollectionNameInHeadersTable()
{
  return ( pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() + " = :" + pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionHeadersTable() );
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForChildCollectionNameInHeadersTable()
{
  return ( pool::RelationalCollection::RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() + " = :" + pool::RelationalCollection::RelationalCollectionNames::childCollectionNameVariableInCollectionHeadersTable() );
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForCollectionNameInDescriptionsTable()
{
  return ( pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() + " = :" + pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForColumnNameInDescriptionsTable()
{
  return ( RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable() + " = :" + RelationalCollectionNames::collectionVariableNameVariableInCollectionDescriptionsTable() + " AND " + RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() + " = :" + RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForCollectionNameInIndexDescriptionsTable()
{
  return ( pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() + " =:" +pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionIndexDescriptionsTable() );
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForIndexNameInIndexDescriptionsTable()
{
  return ( pool::RelationalCollection::RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() + " = :" + pool::RelationalCollection::RelationalCollectionNames::indexNameVariableInCollectionIndexDescriptionsTable() + " AND " + pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() + " = :" + pool::RelationalCollection::RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForUniqueConstraintNameInIndexDescriptionsTable()
{
  return ( RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() + " = :" + RelationalCollectionNames::uniqueConstraintNameVariableInCollectionIndexDescriptionsTable() + " AND " + RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() + " = :" + RelationalCollectionNames::collectionNameVariableInCollectionDescriptionsTable() );
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForPrimaryKeyInDataTable()
{
  return ( pool::RelationalCollection::RelationalCollectionNames::recordIdVariableInCollectionDataTable() + " = :" + pool::RelationalCollection::RelationalCollectionNames::recordIdVariableInCollectionDataTable() );
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::whereClauseForLinkIdInLinksTable()
{
  return ( pool::RelationalCollection::RelationalCollectionNames::linkIdVariableInCollectionLinksTable() + " = :" + pool::RelationalCollection::RelationalCollectionNames::linkIdVariableInCollectionLinksTable() );
}



std::string
pool::RelationalCollection::RelationalCollectionBindVariables::
whereClauseForMetadata()
{
   return RelationalCollectionNames::collectionNameColumn() + " = :" + RelationalCollectionNames::collectionNameColumn();
}


std::string
pool::RelationalCollection::RelationalCollectionBindVariables::
whereClauseForMetadataKey()
{
  return whereClauseForMetadata() + " AND " + RelationalCollectionNames::metadataKeyColumn() + " = :" + RelationalCollectionNames::metadataKeyColumn();
}
