/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_IMPLICITCOLLECTION_IMPLICITCOLLECTION_H
#define INCLUDE_IMPLICITCOLLECTION_IMPLICITCOLLECTION_H

#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionDescription.h"

#ifdef HAVE_GAUDI_PLUGINSVC  
#include "Gaudi/PluginService.h"
#endif 
#include "GAUDI_VERSION.h"

namespace pool {

  // forward declarations
   class ISession;
   class IContainer;
   class ICollectionSchemaEditor;
   class ICollectionDataEditor;
   class ICollectionQuery;
   class ICollectionIterator;
   class ImplicitCollectionIterator;
   class ImplicitCollectionMetadata;

  /// An implicit collection implementation of the ICollection interface
  class ImplicitCollection : virtual public ICollection
  {
  public:
#ifdef HAVE_GAUDI_PLUGINSVC  
  #if GAUDI_VERSION > CALC_GAUDI_VERSION(25, 3) 
    typedef Gaudi::PluginService::Factory<ICollection*, const ICollectionDescription*, ICollection::OpenMode, ISession*> Factory;  
  #else  
    typedef Gaudi::PluginService::Factory3<ICollection*, const ICollectionDescription*, ICollection::OpenMode, ISession*> Factory;  
  #endif
#endif 

    /** Constructor - old style
       Throws POOL exception.
       @param session the session object
       @param connection database connection string. It has the format databaseNameType:databaseName, where databaseNameType can be FID, PFN or LFN.
       @param name the container name in the database
       @param mode collection's open mode. For the moment only READONLY mode is allowed.
    */
    ImplicitCollection( ISession* session,
                        std::string connection,
                        std::string name,
                        ICollection::OpenMode mode );

    /// Constructor compying to the new Collections API
    /// parameters as above, but name and connection passed in description
    ImplicitCollection( const ICollectionDescription* description,
                        ICollection::OpenMode mode,
                        ISession* session );
    
    /// Destructor
    ~ImplicitCollection();

    ImplicitCollection (const ImplicitCollection&) = delete;
    ImplicitCollection& operator= (const ImplicitCollection&) = delete;

    /// Return openMode
    virtual ICollection::OpenMode openMode() const; 
    

    /** Method that returns collection's iterator
         Throws POOL exception.
         @param primaryQuery query string passed to the underlying StorageSvc implementation.
         @param secondaryQuery parameter currently unused
         @param options type currently unused
     */
    ImplicitCollectionIterator* select( std::string primaryQuery = "",
                                        std::string secondaryQuery = "",
                                        std::string options = "" );

    /// Commits the last changes made to the collection. Will always return true.
    void commit(bool reopen=false);

    /// Aborts the last changes made to the collection.  Will always return true.
    void rollback();

    ///  no-op at the moment
    void close();

    ///  no-op at the moment
    void open();

    /// Checks if the collection is open.
    bool isOpen() const;


    virtual bool exists( const std::string& fragmentName,
                         bool setForUpdate = false,
                         bool checkChildFragments = false ) const;

    virtual bool drop( const std::string& fragmentName,
                       bool dropChildFragments = false,
                       bool ignoreExternalDependencies = false );


    virtual bool rename( const std::string& oldName,
                         const std::string& newName );


    virtual bool grantToUser( const std::string& userName,
                              pool::ICollection::Privilege privilege,
                              const std::string& fragmentName = "",
                              bool grantForChildFragments = false );


    virtual bool revokeFromUser( const std::string& userName,
                                 pool::ICollection::Privilege privilege,
                                 const std::string& fragmentName = "",
                                 bool revokeForChildFragments = false );

    virtual bool grantToPublic( const std::string& fragmentName = "",
                                bool grantForChildFragments = false );

    virtual bool revokeFromPublic( const std::string& fragmentName = "",
                                   bool revokeForChildFragments = false );
 


    /// Returns an object used to describe the collection properties.
    virtual const ICollectionDescription& description() const;

    /// Returns an object used to modify the collection schema.
    /// will throw exception if called
    virtual ICollectionSchemaEditor&         schemaEditor();

    /// Returns an object used to add, update or delete rows of the collection.
    /// will throw exception if called
    virtual ICollectionDataEditor&         dataEditor();

    /// Returns an object used to query the collection.
    virtual ICollectionQuery*                 newQuery() const;

    /// Returns an object used to access collection metadata
    virtual ICollectionMetadata&        metadata();

 protected:

    void open( ICollection::OpenMode mode, ISession* session );


  private:
    /// The underlying container handle
    IContainer                        *m_container;

    CollectionDescription        m_description;

    ImplicitCollectionMetadata*        m_metadata;
  };
}

#endif
