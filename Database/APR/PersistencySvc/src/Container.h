/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_CONTAINER_H
#define INCLUDE_PERSISTENCYSVC_CONTAINER_H

// includes
#include "PersistencySvc/IContainer.h"
#include "PersistencySvc/ITechnologySpecificAttributes.h"

namespace pool {

  // forward declarations
  class FileDescriptor;
  class IStorageExplorer;

  namespace PersistencySvc {

    /** @class Container
     *
     *  Container is an implementation of the IContainer interface
     *
     */
    
    class Container : virtual public IContainer,
                      virtual public ITechnologySpecificAttributes {
    public:
      Container( const FileDescriptor& fileDescriptor,
                 IStorageExplorer& storageExplorer,
                 long technology,
                 const std::string& name );
      
      /// destructor
      ~Container();
      
      /// Returns the name (fid) of the parent database
      const std::string& parentDatabaseName() const;

      /// Returns the technology identifier for this container
      long technology() const;

      /** Starts an iteration over the tokens in the container.
       *  Returns a token iterator whose ownership is passed to the user.
       */
      ITokenIterator* tokens( const std::string& selection );
      
      /// Returns the object holding the technology specific attributes for a given technology domain
      const ITechnologySpecificAttributes& technologySpecificAttributes() const;
      ITechnologySpecificAttributes& technologySpecificAttributes();

    protected:
      /// The actual method returning the attribute data given a name
      bool attributeOfType( const std::string& attributeName,
                            void* data,
                            const std::type_info& typeInfo,
                            const std::string& option ) const;

      /// The actual method setting the attribute data given a name
      bool setAttributeOfType( const std::string& attributeName,
                               const void* data,
                               const std::type_info& typeInfo,
                               const std::string& option );

    private:
      /// Reference to file descriptor of the parent database
      const FileDescriptor& m_fileDescriptor;
      
      /// Reference to the storage explorer
      IStorageExplorer& m_storageExplorer;
      
      /// The technology identifier
      long m_technology;
    };
  }
}

#endif
