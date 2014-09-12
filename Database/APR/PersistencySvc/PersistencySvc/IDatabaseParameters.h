/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_IDATABASEPARAMETERS
#define INCLUDE_PERSISTENCYSVC_IDATABASEPARAMETERS

// includes
#include <string>
#include <set>

namespace pool {

  /** @class IDatabaseParameters IDatabaseParameters.h PersistencySvc/IDatabaseParameters.h
   *
   *  IDatabaseParameters is the interface for an object holding annotation-like (string-string pairs) parameters.
   *
   */

  class IDatabaseParameters {
  protected:
    /// Default destructor
    virtual ~IDatabaseParameters() {}

  public:
    /// Returns the names of all the currently available parameters
    virtual std::set< std::string > parameterNames() const = 0;

    /// Returns the value of a parameter given its name. If the parameter does not exist an empty string is returned.
    virtual std::string value( const std::string& name ) const = 0;

    /// Adds a new parameter with a given value.
    virtual void addParameter( const std::string& name, const std::string& value ) = 0;
  };

}

#endif
