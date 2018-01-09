/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//$Id: IPersistencySvc.h 739662 2016-04-12 11:55:10Z krasznaa $

#ifndef PERSISTENCYSVC_IPERSISTENCYSVC
#define PERSISTENCYSVC_IPERSISTENCYSVC

#include <string>
#include <memory>

class Placement;
class Token;
#include "DataModelRoot/RootType.h"

/**
    pool namespace
*/
namespace pool {
  // Forward declarations
  class IFileCatalog;
  class IConfiguration;
  class ISession;

  /** @class IPersistencySvc IPersistencySvc.h PersistencySvc/IPersistencySvc.h
   * 
   *  IPersistencySvc is the abstract interface for all services which execute
   *  the conversion between transient and persistent objects.
   *
   */
  class IPersistencySvc { 
  public:

    /// Factory for PersistencySvc
    static std::unique_ptr<IPersistencySvc> create( IFileCatalog& catalog );

    /// Empty destructor
    virtual ~IPersistencySvc() {}

    /// Returns the file catalog in use
    virtual IFileCatalog& fileCatalog() = 0;

    /// Set the attached file catalog
    virtual void setFileCatalog( IFileCatalog& catalog ) = 0;

    /// Returns the persistency configuration
    virtual IConfiguration& configuration() = 0;

    /** Retrieves an object from persistent store and return with type information
     *  The handle to the reflection class is necessary to later delete the object.
     *  The Guid of the transient class is assumed to be the classID of the token
     *
     * @param  token   [IN]  reference to the token for the object
     * @param  object  [IN]  pointer to memory for the object (created if 0)
     *
     * @return void*   The data.
     *
     * In case of failure zero is returned.
     *
     */
    virtual void* readObject( const Token& token, void* object = 0 ) = 0;

    /**  registerForWrite registers an object for writing to the persistent medium
     *   higher level interactions with the framework are necessary.
     *
     * @param  place        [IN]  the placement hint
     * @param  object       [IN]  pointer to transient object which will be written
     * @param  type         [IN]  reflection class description with the layout of transient object
     *
     * @return Token*   the token address of the persistent object. I case of failure 0 is returned.
     *
     */
    virtual Token* registerForWrite( const Placement& place,
                                     const void* object,
                                     const RootType& type ) = 0;
    
    /** Deletes an object from the persistent store
     *
     * @param  token        [IN]  reference to the token for the object
     *
     * @return bool   Indicates whether operation was successfull.
     *
     */
    virtual bool destroyObject( const Token& token ) = 0;

    /// Returns the container name for object
    virtual std::string getContName( const Token& token ) = 0;

    /// Returns the underlying global session
    virtual ISession& session() = 0;

  protected:
    /// No copy constructor, and no assignment operator
    IPersistencySvc() {}
    IPersistencySvc( const IPersistencySvc& );
    IPersistencySvc& operator=( const IPersistencySvc& );

  }; // class IPersistencySvc

} // namespace pool 

#endif // PERSISTENCYSVC_IPERSISTENCYSVC
