/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_COLLECTIONFACTORY_H
#define COLLECTIONBASE_COLLECTIONFACTORY_H

#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionDescription.h"
#include "FileCatalog/IFileCatalog.h"
#include "CxxUtils/checker_macros.h"

#include <string>
#include <vector>


namespace pool {
   
   class ICollectionDescription;
   class ISession;
   class MetaDataEntry;
   class ICollectionCursor;

   /**
   * @class CollectionFactory CollectionFactory.h CollectionFactory/CollectionFactory.h
   *  
   * A plugin factory for the creation of storage technology specific collections or 
   * collection fragments, the latter of which contain a subset of the metadata of a 
   * full collection.
   */
  class ATLAS_NOT_THREAD_SAFE CollectionFactory
  // not thread-safe due to constness violations wrt the catalog.
  {
  public:
    /// Retrieves the collection factory singleton.
    static CollectionFactory* get();

    /**
     * Creates an instance of a collection or collection fragment, given its storage technology 
     * type. If the collection or collection fragment does not exist a new one can be created. 
     * If it already exists it may be overwritten or opened for read or update transactions.
     *
     * @param type Storage technology type of collection or collection fragment.
     * @param description Specification of collection or collection fragment properties.
     * @param openMode Open mode for collection or collection fragment (CREATE, CREATE_AND_OVERWRITE, READ or UPDATE).
     * @param session Reference to database session (only needed for implicit collections).
     */
    virtual ICollection* create( const ICollectionDescription& description,
                                 ICollection::OpenMode openMode,
                                 ISession* session = 0 ) const;

    /**
     * Creates or overwrites a collection given a description of its properties, and registers 
     * it in a collection catalog. Throws an exception if the collection has been replicated, 
     * if an attempt is being made to overwrite an existing collection in CREATE open mode 
     * or if the object being registered is a collection fragment.
     *
     * @param description Specification of collection properties.
     * @param collectionCatalog Collection catalog manager.
     * @param overwrite Flag indicating whether to overwrite existing collection.
     * @param logicalName Optional logical name assigned to collection in collection catalog.
     * @param metadata Optional collection catalog metadata.
     * @param session Reference to database session (place holder for factory).
     */
    virtual ICollection* createAndRegister( const ICollectionDescription& description,
                                            IFileCatalog* collectionCatalog,
                                            bool overwrite = false,
                                            std::string logicalName = "",
                                            MetaDataEntry* metadata = 0,
                                            ISession* session = 0 ) const;
    
    /**
     * Registers an existing collection in a collection catalog, given a specification of its 
     * properties. Throws an exception if the collection does not exist, if a collection has 
     * already been registered in the catalog with identical physical name, or if the object 
     * being registered is a collection fragment.
     * NOTE: will open the collection - do NOT use if you opened the collection for update
     *   - see the other registerExisting() method
     *
     * @param description Specification of collection properties.
     * @param collectionCatalog Collection catalog manager.
     * @param logicalName Optional logical name assigned to collection in collection catalog.
     * @param metadata Optional collection catalog metadata.
     * @param session Reference to database session (place holder for factory).
     */
    virtual bool registerExisting( const ICollectionDescription& description,
                                   IFileCatalog* collectionCatalog,
                                   std::string logicalName = "",
                                   MetaDataEntry* metadata = 0,
                                   ISession* session = 0 ) const;

    /**
     * Registers an existing collection in a collection catalog, but uses user-supplied
     * exiting collection object
     *
     * @param collection The collection to register
     * @param overwrite If true overwrite catalog entry for the same collection name
     * @param collectionCatalog Collection catalog manager.
     * @param logicalName Optional logical name assigned to collection in collection catalog.
     * @param metadata Optional collection catalog metadata.
     * @param session Reference to database session (place holder for factory).
     */
    virtual bool registerExisting( ICollection* collection,
				   bool overwrite,
                                   IFileCatalog* collectionCatalog,
                                   std::string logicalName = "",
                                   MetaDataEntry* metadata = 0,
                                   ISession* session = 0 ) const;
    
    /**
     * Opens an existing collection for read or update transactions, given its physical name. 
     * Throws an exception if a collection with this physical name cannot be found in the catalog.
     *
     * @param physicalName Physical name of collection in collection catalog.
     * @param collectionCatalog Collection catalog manager.
     * @param openMode The mode in which to open the collection
     * @param session Reference to database session (place holder for factory).
     */
    virtual ICollection* openWithPhysicalName( const std::string& physicalName,
                                               IFileCatalog* collectionCatalog,
                                               ICollection::OpenMode openMode = pool::ICollection::READ,
                                               ISession* session = 0 ) const;
    
    /**
     * Opens an existing collection for read or update transactions, given a logical name 
     * assigned to it in a collection catalog. Throws an exception if a collection with this 
     * logical name cannot be found in the catalog.
     *
     * @param LogicalName Logical name of collection in collection catalog.
     * @param collectionCatalog Collection catalog manager.
     * @param readOnly Flag indicating whether collection is opened for read or update.
     * @param session Reference to database session (place holder for factory).
     */
    virtual ICollection* openWithLogicalName( const std::string& logicalName,
                                              IFileCatalog* collectionCatalog,
                                              bool readOnly = true,
                                              ISession* session = 0 ) const;
    
    /**
     * Opens an existing collection for read or update transactions, given the globally unique 
     * identifier used to register it in a collection catalog. Throws an exception if a collection
     * with this GUID cannot be found in the catalog.
     *
     * @param guid Globally unique identifier of collection in collection catalog.
     * @param catalog Collection catalog manager.
     * @param readOnly Flag indicating whether collection is opened for read or update.
     * @param session Reference to database session (place holder for factory).
     */
    virtual ICollection* openWithGuid( const FileCatalog::FileID& guid,
                                       IFileCatalog* collectionCatalog,
                                       bool readOnly = true,
                                       ISession* session = 0 ) const;



    // resolve PHYSICAL_NAME, LOGICAL_NAME and GUID notation (using catalog)
    virtual void resolveDescription( CollectionDescription& description, bool readOnly = true ) const;
    
    virtual CollectionDescription descFromPhysicalName( const std::string& physicalName,
                                                        IFileCatalog* collectionCatalog,
                                                        bool readOnly ) const;
      
    virtual CollectionDescription descFromLogicalName( const std::string& logicalName,
                                                       IFileCatalog* collectionCatalog,
                                                       bool readOnly ) const;

    virtual CollectionDescription descFromGuid( const FileCatalog::FileID& guid,
                                                IFileCatalog* collectionCatalog,
                                                bool readOnly ) const;

    /// set external file catalog (it will not be deleted)
    /// pass 0 to reset the existing file catalog
    void 		setDefaultCatalog( pool::IFileCatalog* catalog);

    /// set write catalog (once it is set, it can only be changed by resetting)
    void		setWriteCatalog( const std::string &cat );
    
    /// get the default file catalog
    /// if there was no file catalog, new one will be created according
    /// to environment variables POOL_COLLECTION_[WRITE|READ]_CATALOG
    IFileCatalog* 	getDefaultCatalog() const;
  
  private:
    /// Default constructor.
    CollectionFactory();

    /// Default destructor.
    virtual ~CollectionFactory();

    /// Collection factory singleton.
    static CollectionFactory s_instance;

    /// Definition of file catalog entry type for collections registered in catalog.
    static const std::string c_fileType;

    /**
     * Indicates whether the GUID of a collection is unique in a collection catalog.
     *
     * @param guid GUID of collection in collection catalog.
     * @param collectionCatalog Collection catalog manager.
     */
    bool isUnique( const FileCatalog::FileID& guid,
                   IFileCatalog& collectionCatalog ) const;

    /**
     * Extracts the collection storage technology type and database connection string and name 
     * from the physical name of a collection used to store it in a collection catalog.
     *
     * @param type Storage technology type of collection.
     * @param connection Connection to database containing collection.
     * @param name Name of collection.
     * @param physicalName physical name of collection in catalog.
     */    
    void extract( const std::string physicalName,
                  std::string& type, 
                  std::string& connection,
                  std::string& name ) const;    

    
 
    ICollection* 	create_callPlugin( const ICollectionDescription& description,
					   ICollection::OpenMode openMode,
					   ISession* session = 0 ) const;
   
    protected:   
    mutable bool		m_myOwnCatalog;
    mutable IFileCatalog*	m_catalog;  

  };

}

#endif
