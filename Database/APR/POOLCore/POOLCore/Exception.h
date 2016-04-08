/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOL_EXCEPTION_H
#define POOL_EXCEPTION_H

#include <exception>
#include <string>

namespace pool {

  /// Base exception class for the POOL system
  class Exception : public std::exception
  {
  public:
    /// Constructor
    Exception( const std::string& message,
               const std::string& methodName,
               const std::string& moduleName );

    /// Destructor
    virtual ~Exception() throw() {}

    /// The error reporting method
    virtual const char* what() const throw() { return m_message.c_str(); }

  private:
    /// The name of the module which threw the exception
    std::string m_message;
  };
}

#endif
