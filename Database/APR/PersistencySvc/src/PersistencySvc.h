/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENCYSVC_PERSISTENCYSVC
#define PERSISTENCYSVC_PERSISTENCYSVC

// includes
#include "PersistencySvc/IPersistencySvc.h"

namespace pool {

  namespace PersistencySvc {
    // forward declarations
    class UserSession;

    /** @class PersistencySvc
     * 
     * A concrete implementation of the IPersistencySvc interface
     * 
     */
    class PersistencySvc : virtual public IPersistencySvc {
    public:
      /// Constructor
      explicit PersistencySvc( IFileCatalog& fileCatalog );
      
      /// Destructor
      ~PersistencySvc();

      PersistencySvc (const PersistencySvc&) = delete;
      PersistencySvc& operator= (const PersistencySvc&) = delete;

      /// Returns the file catalog in use
      IFileCatalog& fileCatalog();

      /// Set the attached file catalog
      void setFileCatalog( IFileCatalog& catalog );

      /// Returns the persistency configuration
      IConfiguration& configuration();
      
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
      void* readObject( const Token& token, void* object = 0 );
      
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
      Token* registerForWrite( const Placement& place,
                               const void* object,
                               const RootType& type );
      
      /** Deletes an object from the persistent store
       *
       * @param  token        [IN]  reference to the token for the object
       *
       * @return bool   Indicates whether operation was successfull.
       *
       */
      bool destroyObject( const Token& token );
      
      /// Returns the container name for object
      std::string getContName( const Token& token );

      /// Returns the underlying global session
      ISession& session();

    private:
      /// The session object.
      UserSession* m_session;

    };
  }
}

#endif // PERSISTENCYSVC_PERSISTENCYSVC
