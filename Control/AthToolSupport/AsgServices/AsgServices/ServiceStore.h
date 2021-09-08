/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author David Adams <dladams@bnl.gov> (for original implementation for tools)
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch> (for original implementation for tools)



#ifndef ASGSERVICES_SERVICESTORE_H
#define ASGSERVICES_SERVICESTORE_H

#ifndef XAOD_STANDALONE
#error "This header should only be used in XAOD_STANDALONE"
#else

// System include(s):
#include <memory>
#include <string>

// Local include(s):
#include "AsgServices/IAsgService.h"
#include "AsgMessaging/StatusCode.h"

namespace asg
{

  /// A light-weight store for services in stand-alone analysis
  ///
  /// In order to allow users to find services through
  /// ServiceHandles, just like in Athena, we need a global registry
  /// of all of the services that the user instantiates in his/her
  /// job.
  ///
  /// This class does exactly this. Services created via \ref
  /// AsgServiceConfig get registered here so that other components
  /// can find them.
  ///
  /// Based on \ref ToolStore

  class ServiceStore final
  {
  public:
    /// Store a named service by its name.
    /// The name must be non-blank and not already exist in the store.
    static StatusCode put( const std::shared_ptr<IAsgService>& pservice );

    /// Retrieve a service by name.
    static std::shared_ptr<IAsgService> get( const std::string& name, bool silent = false );

    /// Retrieve a service by name and interface.
    template< typename T >
    // cppcheck-suppress constStatement; false positive
    static std::shared_ptr<T> get( const std::string& name )
    {
      return std::dynamic_pointer_cast< T >( get( name ) );
    }

    /// Check whether a service with a given type and name is known in the store
    template< typename T >
    static bool contains( const std::string& name )
    {
      return ( dynamic_cast< T* >( get( name, true ).get() ) != 0 );
    }

    /// dump the service configuration for all services to std::cout
    static void dumpServiceConfig ();

  }; // class ServiceStore

} // namespace asg

#endif

#endif // ASGSERVICES_SERVICESTORE_H
