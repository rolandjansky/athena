/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTIONBINDVARIABLES_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTIONBINDVARIABLES_H

#include <string>

namespace pool {

  namespace RelationalCollection {

    /// Utility class holding frequently used bind variables in the RelationalCollection package.
    class RelationalCollectionBindVariables 
    {
    public:
      // Collection headers table.

      // Returns a where clause for finding a collection fragment name in the collection headers table.
      static std::string whereClauseForCollectionNameInHeadersTable();
      // Returns a where clause for finding a child collection fragment name in the collection headers table.
      static std::string whereClauseForChildCollectionNameInHeadersTable();

      // Collection descriptions table.

      /// Returns a where clause for finding a collection fragment name in the collection descriptions table.
      static std::string whereClauseForCollectionNameInDescriptionsTable();
      /// Returns a where clause for finding a column name in the collection descriptions table.
      static std::string whereClauseForColumnNameInDescriptionsTable();

      // Collection indices and unique constraints table.

      /// Returns a where clause for finding a collection fragment name in the index and unique constraints descriptions table.
      static std::string whereClauseForCollectionNameInIndexDescriptionsTable();
      /// Returns a where clause for finding an index name in the index and unique constraint descriptions table.
      static std::string whereClauseForIndexNameInIndexDescriptionsTable();
      /// Returns a where clause for finding a unique constrant name in the index and unique constraint descriptions table.
      static std::string whereClauseForUniqueConstraintNameInIndexDescriptionsTable();

      // Collection data table.

      /// Returns a where clause for finding a primary key in the data table of a collection.
      static std::string whereClauseForPrimaryKeyInDataTable();

      // Collection links table.

      /// Returns a where clause for finding a link ID in the links table of a collection.
      static std::string whereClauseForLinkIdInLinksTable();

      
      // Metadata table
      static std::string whereClauseForMetadata();
      static std::string whereClauseForMetadataKey();

    };
  }
}

#endif
