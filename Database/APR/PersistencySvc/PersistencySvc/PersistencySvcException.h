/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENCYSVC_PERSISTENCYSVC_EXCEPTION_H
#define PERSISTENCYSVC_PERSISTENCYSVC_EXCEPTION_H

#include "POOLCore/Exception.h"

namespace pool {

  /// Exception class thrown by the classes in the PersistencySvc package
  class PersistencySvcException : public pool::Exception {
  public:
    /// Constructor
    PersistencySvcException( const std::string& message,
                             const std::string& methodName ):
      pool::Exception( message, methodName, "PersistencySvc" )
    {}
  };

}

#endif
