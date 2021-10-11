/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
      Container( FileDescriptor& fileDescriptor,
                 IStorageExplorer& storageExplorer,
                 long technology,
                 const std::string& name );
      
      /// destructor
      ~Container();
      
      /// Returns the name (fid) of the parent database
      virtual const std::string& parentDatabaseName() const override;

      /// Returns the technology identifier for this container
      virtual long technology() const override;

      /** Starts an iteration over the tokens in the container.
       *  Returns a token iterator whose ownership is passed to the user.
       */
      virtual ITokenIterator* tokens( const std::string& selection ) override;
      
      /// Returns the object holding the technology specific attributes for a given technology domain
      virtual const ITechnologySpecificAttributes& technologySpecificAttributes() const override;
      virtual ITechnologySpecificAttributes& technologySpecificAttributes() override;

    protected:
      /// The actual method returning the attribute data given a name
      virtual
      bool attributeOfType( const std::string& attributeName,
                            void* data,
                            const std::type_info& typeInfo,
                            const std::string& option ) override;

      /// The actual method setting the attribute data given a name
      virtual
      bool setAttributeOfType( const std::string& attributeName,
                               const void* data,
                               const std::type_info& typeInfo,
                               const std::string& option ) override;

    private:
      /// Reference to file descriptor of the parent database
      FileDescriptor& m_fileDescriptor;
      
      /// Reference to the storage explorer
      IStorageExplorer& m_storageExplorer;
      
      /// The technology identifier
      long m_technology;
    };
  }
}

#endif
