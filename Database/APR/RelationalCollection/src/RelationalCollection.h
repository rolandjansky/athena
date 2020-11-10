/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_RELATIONALCOLLECTION_H
#define RELATIONALCOLLECTION_RELATIONALCOLLECTION_H

#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/ICollectionRelationalExtensions.h"

#include "RelationalCollectionFragmentData.h"

#include "Gaudi/PluginService.h"

#include <map>


namespace coral {
   class ISessionProxy;
   class ISchema;
   class AttributeList;
}


namespace pool {

   class ISession;
   
  namespace RelationalCollection {

     class RelationalCollectionSchemaEditor;
     class RelationalCollectionDataEditor;
     class RelationalCollectionMetadata;

    /**
     * @class RelationalCollection RelationalCollection.h src/RelationalCollection.h
     *
     * Implementation of the ICollection interface to a collection of event references 
     * and their associated metadata. This class may also be used as an interface to a 
     * "collection fragment" which, by definition, contains a subset of the event level 
     * metadata, does not contain the event reference and which can reference an existing 
     * collection or collection fragment and/or be referenced by a chain of collection 
     * fragments to form a collection or collection fragment with extended schema.
     */
     class RelationalCollection : virtual public ICollection, public ICollectionRelationalExtensions
    {
    public:
      typedef Gaudi::PluginService::Factory<ICollection*( const ICollectionDescription*, ICollection::OpenMode, ISession*)> Factory;

      /** 
       * Constructor.
       *
       * @param description Specication of collection properties.
       * @param mode Open mode of collection (CREATE, CREATE_MISSING_FRAGMENTS, CREATE_AND_OVERWRITE, READ or UPDATE).
       * @param session Reference to POOL database session (place holder for implicit collections).
       */
      RelationalCollection( const pool::ICollectionDescription* description,
                            pool::ICollection::OpenMode mode,
                            pool::ISession* session );

      /// Default destructor.
      ~RelationalCollection();

      RelationalCollection (const RelationalCollection&) = delete;
      RelationalCollection& operator= (const RelationalCollection&) = delete;
    
      /// Returns the open mode of the collection for the present transaction.
      virtual ICollection::OpenMode openMode() const;

      /// Opens the collection and initializes it if necessary.
      virtual void open();

      /// Checks if the collection is open.
      virtual bool isOpen() const;

      /// Commits the latest changes made to the collection.
      virtual void commit( bool restartTransaction = true );

      /// Aborts the latest changes made to the collection.
      virtual void rollback();

      /// Closes the collection and terminates any database connections.
      virtual void close();

      /**
       * Checks for the existence of one or more fragments of the collection.
       *
       * @param fragmentName Name of collection fragment.
       * @param setForUpdate Flag indicating whether to lock the collection for update if it exists.
       * @param checkChildFragments Flag to check for existence of all child fragments of given fragment.
       */
      virtual bool exists( const std::string& fragmentName,
                           bool setForUpdate = false,
                           bool checkChildFragments = false ) const;

      /**
       * Drops one or more fragments of the collection and returns false if a fragment to be dropped cannot be found.
       *
       * @param fragmentName Name of collection fragment to drop.
       * @param dropChildFragments Flag indicating whether to drop all child fragments of given fragment.
       * @param ignoreExternalDependencies Flag indicating whether to check for dependencies on fragment by external fragments.
       */
      virtual bool drop( const std::string& fragmentName,
                         bool dropChildFragments = false,
                         bool ignoreExternalDependencies = false );

        
      /// Actuall implementation of drop method, with additional parameter for commit at the end
      bool drop_impl( const std::string& fragmentName,
                      bool dropChildFragments = false,
                      bool ignoreExternalDependencies = false,
                      bool commit = true );


       /**
       * Renames a fragment of the collection.
       *
       * @param oldName Old name of collection fragment.
       * @param newName New name of collection fragment.
       */
      virtual bool rename( const std::string& oldName,
                           const std::string& newName );

      /*
       * Grants an access privilege to a user for one or more collection fragments.
       *
       * @param userName Name of user to which to grant the privilege.
       * @param privilege Type of privilege to grant.
       * @param fragmentName Name of collection fragment to which to grant access.
       * @param grantForChildFragments Flag indicating whether to grant same access for all child fragments of specified collection fragment.
       */
      virtual bool grantToUser( const std::string& userName,
                                pool::ICollection::Privilege privilege,
                                const std::string& fragmentName = "",
                                bool grantForChildFragments = false );

      /*
       * Revokes an access privilege from a user for one or more collection fragments.
       *
       * @param userName Name of user from which to revoke the privilege.
       * @param privilege Type of privilege to revoke.
       * @param fragmentName Name of collection fragment from which to revoke access.
       * @param revokeForChildFragments Flag indicating whether to revoke same access for all child fragments of specified collection fragment.
       */
      virtual bool revokeFromUser( const std::string& userName,
                                   pool::ICollection::Privilege privilege,
                                   const std::string& fragmentName = "",
                                   bool revokeForChildFragments = false );

      /*
       * Grants SELECT access to the public for one or more collection fragments.
       *
       * @param fragmentName Name of collection fragment for which to grant access.
       * @param grantForChildFragments Flag indicating whether to grant same access for all child fragments of specified collection fragment.
       */
      virtual bool grantToPublic( const std::string& fragmentName = "",
                                  bool grantForChildFragments = false );

      /*
       * Revokes SELECT access from the public for one or more collection fragments.
       *
       * @param fragmentName Name of collection fragment for which to revoke access.
       * @param revokeForChildFragments Flag indicating whether to revoke same access for all child fragments of specified collection fragment.
       */
      virtual bool revokeFromPublic( const std::string& fragmentName = "",
                                     bool revokeForChildFragments = false );

      /// Returns an object used to describe the collection properties.
      virtual const ICollectionDescription& description() const;

      /// Returns an object used to modify the collection schema.
      virtual ICollectionSchemaEditor& schemaEditor();

      /// Returns an object used to add, update or delete rows of the collection.
      virtual ICollectionDataEditor& dataEditor();

      /// Returns an object used to query the collection.
      virtual ICollectionQuery* newQuery() const;

      /// Returns an object used to access collection metadata
      virtual ICollectionMetadata&        metadata();

      /// Returns an object used to query the collection for GUIDs directly on the server
      virtual ICollectionGUIDQuery* newGUIDQuery() const;

   private:
      /// Initializes the collection.
      void initialize();

      /// Creates the collection headers table.
      void createHeadersTable();

      /// Creates the collection descriptions table.
      void createDescriptionsTable();

      /// Creates the collection index and unique constraint descriptions table.
      void createIndexDescriptionsTable();

      /**
       * Checks whether the schema of a collection fragment defined by the collection description
       * object matches the schema of an existing collection fragment of the same name.
       *
       * @param fragmentName Name of the collection fragments.
       */
      bool newSchemaEqualsExistingSchema( const std::string& fragmentName );

      /**
       * Creates the data and links tables for a fragment of a collection being created.
       *
       * @param fragmentName Name of collection fragment for which to create tables.
       */
      void createDataAndLinksTables( const std::string& fragmentName );

      /**
       * Initializes the collection headers table for a fragment of a collection being created.
       *
       * @param fragmentName Name of collection fragment for which to initialize collection headers table.
       */
      void initializeHeadersTable( const std::string& fragmentName,
                                   const std::string& dataTableName,
                                   const std::string& linksTableName);

      /**
       * Initializes the collection descriptions table for a fragment of a collection being created.
       *
       * @param fragmentName Name of collection fragment for which to initialize collection descriptions table.
       */
      void initializeDescriptionsTable( const std::string& fragmentName );

      /// Retrieves descriptions of all collection fragments of an existing collection.
      void retrieveCollectionFragmentDescriptions();

      /**
       * Retrieves descriptions of all columns of a collection fragment of an existing collection.
       *
       * @param fragmentName Name of collection fragment for which to retrieve column descriptions.
       * @param collDesc     Description object which is filled with the information.
       */
      void retrieveColumnDescriptions( const std::string& fragmentName,
                                       CollectionDescription *collDesc ) const;

      void retrieveFragmentTableNames( const std::string& fragmentName );

      /**
       * Initializes all private maps and data row buffers used by the collection to add, update or query data.
       *
       * @param fragmentName Name of collection fragment for which to initialize maps and row buffers.
       */
      void initializeMapsAndDataRowBuffers( const std::string& fragmentName );

      /// Creates a row buffer that uses the schema of the Token links tables.
      void createLinksTableRowBuffer();

      /**
       * Creates the maps of link ID's to Token keys for a given fragment of the collection.
       *
       * @param fragmentName Name of collection fragment for which to create maps.
       */
      void createLinkIdToTokenKeyMaps( const std::string& fragmentName );

      /**
       * Creates an index one or more columns of a collection being created. 
       * 
       * @param indexName Name assigned to index.
       * @param columnNames Names of columns for which index is created.
       * @param isUnique Flag to indicates whether combination of indexed column values must be unique.
       */
      void createIndex( const std::string& indexName,
                        const std::vector< std::string >& columnNames,
                        bool isUnique );

      /**
       * Sets a unique constraint on one or more columns of a collection being created.
       *
       * @param constraintName Name of constraint.
       * @param columnNames Names of columns for which constraint is applied.
       */
      void setUniqueConstraint( const std::string& constraintName,
                                const std::vector< std::string >& columnNames );

      /**
       * Retrieves descriptions of all indices in a collection fragment of the collection.
       *
       * @param fragmentName Name of collection fragment that contains the indices.
       */
      void retrieveIndexDescriptions( const std::string& fragmentName );

      /**
       * Retrieves a description of a unique constraint in a collection fragment of the collection.
       *
       * @param fragmentName Name of collection fragment that contains the unique constraints.
       */
      void retrieveUniqueConstraintDescriptions( const std::string& fragmentName );

      /**
       * Drops all foreign key constraints applied by a collection fragment except the constraint
       * (if it exists) that is defined in the collection description. An input flag is provided to 
       * allow the defined constraint to also be dropped.
       * 
       * @param fragmentName Name of collection fragment for which to drop applied foreign key constraints.
       * @param dropDefinedConstraint Flag indicating whether to drop applied constraint defined in collection description. 
       */
      void dropParentForeignKeyConstraints( const std::string& fragmentName,
                                            bool dropDefinedConstraint = false );

      /**
       * Drops the foreign key constraint on the collection fragment by its child collection fragment if such
       * a constraint exists.
       * 
       * @param fragmentName Name of collection fragment for which to drop the foreign key constraint.
       */
      void dropChildForeignKeyConstraint( const std::string& fragmentName );

      /**
       * Sets a foreign key constraint on the parent collection fragment of the collection fragment specified as input.
       * 
       * @param fragmentName Name of collection fragment for which to set constraint.
       */
      void setForeignKeyConstraint( const std::string& fragmentName );

    private:
      /// Specification of collection properties.
      CollectionDescription             m_description;

      /// Open mode of collection for present transaction.
      ICollection::OpenMode             m_mode;

      /// Reference to current database access session.
      coral::ISessionProxy              *m_session;

      /// Name of top level collection fragment.
      std::string                       m_name;

      /// Schema editor for collection.
      RelationalCollectionSchemaEditor  *m_schemaEditor;

      /// Data editor for collection.
      RelationalCollectionDataEditor    *m_dataEditor;

      RelationalCollectionMetadata      *m_metadata;
 

      /// Where clause for finding collection fragment name in collection headers table.
      std::string m_whereClauseForCollectionNameInHeadersTable;

      /// Where clause for finding child collection fragment name in collection headers table.
      std::string m_whereClauseForChildCollectionNameInHeadersTable;

      /// Where clause for finding collection fragment name in collection descriptions table.
      std::string m_whereClauseForCollectionNameInDescriptionsTable;

      /// Where clause for finding collection fragment name in index and unique constraints descriptions table.
      std::string m_whereClauseForCollectionNameInIndexDescriptionsTable;

      // Where clause for finding index name in index and unique constraint descriptions table.
      std::string m_whereClauseForIndexNameInIndexDescriptionsTable;

      /// Bind data for collection headers table collection fragment name where clause.
      coral::AttributeList* m_whereDataForCollectionNameInHeadersTable;

      /// Bind data for collection headers table child collection fragment name where clause.
      coral::AttributeList* m_whereDataForChildCollectionNameInHeadersTable;

      /// Bind data for collection descriptions table collection fragment name where clause.
      coral::AttributeList* m_whereDataForCollectionNameInDescriptionsTable;

      /// Bind data for collection index descriptions table collection fragment name where clause.
      coral::AttributeList* m_whereDataForCollectionNameInIndexDescriptionsTable;

      /// Bind data for collection index descriptions table index name where clause.
      coral::AttributeList* m_whereDataForIndexNameInIndexDescriptionsTable;

      /// Map of data table names using names of corresponding collection fragment names as keys.
      std::map< std::string, std::string > m_dataTableNameForCollectionFragmentName;

      /// Map of links table names using names of corresponding collection fragment names as keys.
      std::map< std::string, std::string > m_linksTableNameForCollectionFragmentName;

      /// Map holding the number of records written for a collection fragment name
      //std::map< std::string, unsigned > m_recordsWrittenForCollectionFragmentName;

      /// Information related to fragments, map keyed by fragment name
      FragmentDataMap                m_fragmentDataMap;

      /// Map of table column prefixes for Tokens using names of corresponding collection columns as keys.
      std::map< std::string, std::string > m_tableTokenColumnPrefixForCollectionTokenColumnName;

      /// Map of table column names for Attributes using names of corresponding collection columns as keys.
      std::map< std::string, std::string > m_tableAttributeColumnNameForCollectionAttributeColumnName;

      /// Map of table column names or prefixes using names of corresponding collection columns as keys.
      std::map< std::string, std::string > m_tableColumnNameForCollectionColumnName;

      /// Map of collection column names using positions of corresponding table columns as keys.
      std::map< int, std::string > m_collectionColumnNameForTableColumnPosition;

      /// List of all Attributes defined by collection but using table column names.
      coral::AttributeList* m_tableAttributeList;
  
      /// Collection row buffer containing all Tokens and Attributes defined by collection.
      pool::CollectionRowBuffer  m_collectionRowBuffer;
  
      /// Map of data table row buffers using names of corresponding collection fragments as keys.
      std::map< std::string, coral::AttributeList* > m_dataTableRowBufferForCollectionFragmentName;

      /// Row buffer using schema of collection links tables.
      coral::AttributeList* m_linksTableRowBuffer;

      /// Map of maps of link table ID's that use corresponding Token prefixes as keys, using collection fragment names as keys.
      std::map< std::string, std::map< std::string, unsigned >* > m_mapOfLinkIdForTokenKeyMaps;

      /// Map of maps of Token prefixes that use corresponding link table ID's as keys, using collection fragment names as keys.
      std::map< std::string, std::map< unsigned, std::string >* > m_mapOfTokenKeyForLinkIdMaps;

      /// Map of where clauses for finding Token OID_1 values in data tables, using Token collection column names as keys.
      std::map< std::string, std::string > m_mapOfWhereClausesForOID1InDataTable;

      /// Map of where data for finding Token OID_1 values in data tables, using Token collection column names as keys.
      std::map< std::string, coral::AttributeList* > m_mapOfWhereDataForOID1InDataTable;

      /// Flag indicating whether collection has been initialized.
      bool m_initialized;

      /// output stream
      // mutable coral::MessageStream                m_poolOut;
    };
     
  }
}

#endif
