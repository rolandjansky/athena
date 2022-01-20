/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author David Adams <dladams@bnl.gov> (for original implementation for tools)
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch> (for original implementation for tools)



// Local include(s):
#include "AsgServices/ServiceStore.h"
#include "AsgServices/AsgService.h"
#include "AsgTools/MessageCheckAsgTools.h"
#include "AsgMessaging/MsgStreamMacros.h"

// System include(s):
#include <map>
#include <iostream>
#include <mutex>

/// Convenience type definition
typedef std::map< std::string, std::weak_ptr<asg::IAsgService> > ServiceMap_t;

namespace
{
  /// The application-wide service registry
  static ServiceMap_t s_services;
  std::mutex s_serviceMutex;
}

namespace asg
{
  StatusCode ServiceStore::
  put( const std::shared_ptr<IAsgService>& pservice )
  {
    using namespace msgComponentConfig;

    // Start with a little sanity check:
    if( ! pservice )
    {
      ANA_MSG_ERROR( "asg::ServiceStore::put      " <<
                     "Received a null pointer" );
      return StatusCode::FAILURE;
    }

    // Get and check the name of the service:
    const std::string& name = pservice->name();
    if( ! name.size() )
    {
      ANA_MSG_ERROR( "asg::ServiceStore::put      " <<
                     "The received service doesn't have a name" );
      return StatusCode::FAILURE;
    }

    std::lock_guard<std::mutex> lock (s_serviceMutex);

    auto& currentService = s_services[name];
    if (currentService.lock())
    {
      ANA_MSG_ERROR ("Service with name \""
                     << name << "\" already registered");
      return StatusCode::FAILURE;
    }

    currentService = pservice;
    return StatusCode::SUCCESS;
  }



  std::shared_ptr<IAsgService> ServiceStore::
  get( const std::string& name, bool silent )
  {
    using namespace msgToolHandle;

    std::lock_guard<std::mutex> lock (s_serviceMutex);
    auto iservice = s_services.find( name );

    std::shared_ptr<IAsgService> result;
    if( iservice != s_services.end() )
      result = iservice->second.lock();

    if( ! silent && !result )
      ANA_MSG_ERROR ("Service with name \"" << name << "\" not found");
    return result;
  }



  void ServiceStore::dumpServiceConfig ()
  {
    using namespace asg::msgProperty;

    // I am first putting everything into a map, so that the error
    // messages don't interleave the property values
    std::map<std::string,std::map<std::string,std::string> > properties;

    std::lock_guard<std::mutex> lock (s_serviceMutex);
    for (auto& service : s_services)
    {
      auto myservice = service.second.lock();
      if (myservice)
      {
        auto& myproperties = properties[service.first];
        myproperties[""] = std::string (typeid(*myservice).name()) + "/" + service.first;
        for (auto& property : dynamic_cast<const AsgService*>(myservice.get())->getPropertyMgr()->getProperties())
        {
          std::string asString;
          if (property.second->getString (asString).isFailure())
          {
            ANA_MSG_ERROR ("on property " << property.first << " for service " << service.first);
            myproperties[property.first] = "<<invalid>>";
          } else
            myproperties[property.first] = asString;
        }
      }
    }
    // Using std::cout here intentionally to bypass the messaging
    // system to avoid accidental changes to the formatting.  This
    // could be updated in the future to allow the user to pass in the
    // stream to output to, if desired.
    for (const auto& myproperties : properties)
    {
      for (auto& property : myproperties.second)
      {
	std::cout << myproperties.first;
	if (!property.first.empty())
	  std::cout << "." << property.first;
	std::cout << " = " << property.second << "\n";
      }
    }
    std::cout << std::flush;
  }

} // namespace asg
