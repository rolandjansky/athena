/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_ICONTAINER_H
#define INCLUDE_PERSISTENCYSVC_ICONTAINER_H

// includes
#include <string>

namespace pool {

  // forward declarations
  class ITokenIterator;
  class ITechnologySpecificAttributes;

  /** @class IContainer IContainer.h PersistencySvc/IContainer.h
   *
   *  IContainer is the base class for container objects
   *
   */

  class IContainer {
  public:
    /// Empty destructor
    virtual ~IContainer() {}

    /// Returns the name of this container
    const std::string& name() const;

    /// Returns the name (fid) of the parent database
    virtual const std::string& parentDatabaseName() const = 0;

    /// Returns the technology identifier for this container
    virtual long technology() const = 0;

    /** Starts an iteration over the tokens in the container.
     *  Returns a token iterator whose ownership is passed to the user.
     */
    virtual ITokenIterator* tokens( const std::string& selection ) = 0;

    /// Returns the object holding the technology specific attributes for a given technology domain
    virtual const ITechnologySpecificAttributes& technologySpecificAttributes() const = 0;
    virtual ITechnologySpecificAttributes& technologySpecificAttributes() = 0;

  protected:
    /// Constructor initializing its name
    explicit IContainer( const std::string& name );

  private:
    /// The name of the container
    std::string m_name;
  };
}

#endif
