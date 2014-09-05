/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTION_H
#define COLLECTIONBASE_ICOLLECTION_H

#include <string>

namespace pool {

  class ICollectionDescription;
  class ICollectionSchemaEditor;
  class ICollectionDataEditor;
  class ICollectionQuery;
  class ICollectionMetadata;

  /** 
   * @class ICollection ICollection.h CollectionBase/ICollection.h
   *
   * An interface to a storage technology specific collection of event references and 
   * their associated metadata. This class may also be used as an interface to a 
   * "collection fragment" which, by definition, contains a subset of the event level 
   * metadata, does not contain the event reference and which can reference an existing 
   * collection or collection fragment and/or be referenced by a chain of collection 
   * fragments to form a collection or collection fragment with extended schema.
   */
   class ICollection
  {
  public:
    /// Enumeration of the possible open modes of the collection.
    typedef enum { CREATE,
                   CREATE_MISSING_FRAGMENTS,
		   CREATE_AND_OVERWRITE,
		   UPDATE,
		   READ } OpenMode;

    /// Enumeration of the possible types of allowable accesses to the collection.
    typedef enum { READ_ACCESS,
                   UPDATE_ACCESS,
		   WRITE_ACCESS,
		   DELETE_ACCESS } Privilege;

    /// Returns the open mode of the collection for the present transaction.
    virtual ICollection::OpenMode openMode() const = 0;

    /// Returns the type of access to the collection that is allowed for the present transaction.
    // TODO
    //    virtual const ICollection::Privilege privilege() = 0;

    /// Opens the collection and initializes it if necessary.
    virtual void open() = 0;

    /// Checks if the collection is open.
    virtual bool isOpen() const = 0;

    /// Commits the latest changes made to the collection.
    virtual void commit( bool restartTransaction = true ) = 0;

    /// Aborts the latest changes made to the collection.
    virtual void rollback() = 0;

    /// Closes the collection and terminates any database connections.
    virtual void close() = 0;

    /**
     * Checks for the existence of one or more fragments of the collection.
     *
     * @param fragmentName Name of collection fragment.
     * @param setForUpdate Flag indicating whether to lock the collection for update if it exists.
     * @param checkChildFragments Flag to check for existence of all child fragments of given fragment.
     */
    virtual bool exists( const std::string& fragmentName,
                         bool setForUpdate = false,
                         bool checkChildFragments = false ) const = 0;

    /**
     * Drops one or more fragments of the collection and returns false if a fragment to be dropped cannot be found.
     *
     * @param fragmentName Name of collection fragment to drop.
     * @param dropChildFragments Flag indicating whether to drop all child fragments of given fragment.
       * @param ignoreExternalDependencies Flag indicating whether to check for dependencies on fragment by external fragments.
     */
    virtual bool drop( const std::string& fragmentName,
                       bool dropChildFragments = false,
                       bool ignoreExternalDependencies = false ) = 0;

    /**
     * Renames a fragment of the collection.
     *
     * @param oldName Old name of collection fragment.
     * @param newName New name of collection fragment.
     */
    virtual bool rename( const std::string& oldName,
                         const std::string& newName ) = 0;

    /*
     * Grants an access privilege to a user for one or more collection fragments.
     *
     * @param userName Name of user to which to grant the privilege.
     * @param privilege Type of privilege to grant.
     * @param fragmentName Name of collection fragment for which to grant access.
     * @param grantForChildFragments Flag indicating whether to grant same access for all child fragments of specified collection fragment.
     */
    virtual bool grantToUser( const std::string& userName,
                              pool::ICollection::Privilege privilege,
                              const std::string& fragmentName = "",
                              bool grantForChildFragments = false ) = 0;

    /*
     * Revokes an access privilege from a user for one or more collection fragments.
     *
     * @param userName Name of user from which to revoke the privilege.
     * @param privilege Type of privilege to revoke.
     * @param fragmentName Name of collection fragment for which to revoke access.
     * @param revokeForChildFragments Flag indicating whether to revoke same access for all child fragments of specified collection fragment.
     */
    virtual bool revokeFromUser( const std::string& userName,
                                 pool::ICollection::Privilege privilege,
                                 const std::string& fragmentName = "",
                                 bool revokeForChildFragments = false ) = 0;

    /*
     * Grants SELECT access to the public for one or more collection fragments.
     *
     * @param fragmentName Name of collection fragment for which to grant access.
     * @param grantForChildFragments Flag indicating whether to grant same access for all child fragments of specified collection fragment.
     */
    virtual bool grantToPublic( const std::string& fragmentName = "",
                                bool grantForChildFragments = false ) = 0;

    /*
     * Revokes SELECT access from the public for one or more collection fragments.
     *
     * @param fragmentName Name of collection fragment for which to revoke access.
     * @param revokeForChildFragments Flag indicating whether to revoke same access for all child fragments of specified collection fragment.
     */
    virtual bool revokeFromPublic( const std::string& fragmentName = "",
                                   bool revokeForChildFragments = false ) = 0;

    /// Returns an object used to describe the collection properties.
    virtual const ICollectionDescription& description() const = 0;

    /// Returns an object used to modify the collection schema.
    virtual ICollectionSchemaEditor& 	schemaEditor() = 0;

    /// Returns an object used to add, update or delete rows of the collection.
    virtual ICollectionDataEditor& 	dataEditor() = 0;

    /// Returns an object used to query the collection.
    virtual ICollectionQuery* 		newQuery() const = 0;
    
    /// Returns an object used to access collection metadata
    virtual ICollectionMetadata&	metadata() = 0;
    
    /// Empty destructor.
    virtual ~ICollection() {}

  };

}

#endif
