/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef ASG_SERVICES_ASG_SERVICE_CONFIG_H
#define ASG_SERVICES_ASG_SERVICE_CONFIG_H

#include <AsgTools/AsgComponentConfig.h>
#include <AsgServices/AsgService.h>
#include <AsgServices/ServiceHandle.h>

#ifndef XAOD_STANDALONE
#include <GaudiKernel/Service.h>
#endif

namespace asg
{
  /// \brief an object that can create a \ref AsgService

  class AsgServiceConfig : public asg::AsgComponentConfig
  {
    //
    // public interface
    //

    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    AsgServiceConfig () = default;


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AsgServiceConfig (const std::string& val_typeAndName);


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    explicit AsgServiceConfig (const AsgComponentConfig& val_config);


    /// \brief Virtual destructor, to make PyROOT happy
    ///
    /// Without it ROOT 6.22+ does not allow Python classes to inherit from this
    /// type.
    ///
  public:
    virtual ~AsgServiceConfig () = default;


    /// \brief make a service with the given configuration
    ///
    /// \warn This is mostly meant as a low level interface to be used
    /// in unit tests and internally by framework functions that
    /// manage services.  As a user you should mostly rely on the
    /// python configuration or on passing the AsgServiceConfig object
    /// to your framework, which will then create and manage the
    /// service for you.
    ///
    /// \warn Regardless of which function you use, the memory
    /// management and cleanup of services differs between stand-alone
    /// use and Athena.  In EventLoop you receive an owning
    /// pointer/handle that can be shared with other users and will
    /// release the service when the last reference is deleted.  In
    /// Athena the framework itself will hold on to the service (or at
    /// least its configuration) and release it at the end of the job,
    /// even if no user code references it.  As a user you should not
    /// rely on the service still being around after you destroy the
    /// last reference, or on the service being around after framework
    /// teardown has started.
    ///
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   configuration errors\n
    ///   service creation/initialization errors
    /// \{
  public:
    template<typename T> ::StatusCode
    makeService (std::shared_ptr<T>& service) const;
    template<typename T> ::StatusCode
    makeService (ServiceHandle<T>& service) const;
    /// \}



    //
    // private interface
    //
  };



  //
  // template methods
  //

#ifdef XAOD_STANDALONE

  template<typename T> ::StatusCode AsgServiceConfig ::
  makeService (std::shared_ptr<T>& service) const
  {
    using namespace msgComponentConfig;

    std::unique_ptr<T> myservice;
    ANA_CHECK (makeComponentExpert (myservice, "new %1% (\"%2%\", nullptr)", false, ""));
    AsgService *asgService = dynamic_cast<AsgService*>(myservice.get());
    ANA_CHECK (asgService->initialize());

    service = std::move (myservice);
    ANA_CHECK (ServiceStore::put (service));

    ANA_MSG_DEBUG ("Created component of type " << type());
    return StatusCode::SUCCESS;
  }

  template<typename T> ::StatusCode AsgServiceConfig ::
  makeService (ServiceHandle<T>& service) const
  {
    using namespace msgComponentConfig;

    std::shared_ptr<T> myservice;
    ANA_CHECK (makeService (myservice));
    service = ServiceHandle (myservice, service.parentName());
    return StatusCode::SUCCESS;
  }

#else

  template<typename T> ::StatusCode AsgServiceConfig ::
  makeService (std::shared_ptr<T>& service) const
  {
    using namespace msgComponentConfig;

    ServiceHandle<T> serviceHandle (typeAndName(), "AsgServiceConfig");
    ANA_CHECK (makeService (serviceHandle));

    // This creates an unmanaged shared pointer, i.e. a pointer that
    // doesn't actually own the pointed to object, it will neither
    // guarantee to keep the object around until its own destruction,
    // nor will it ever trigger a destruction of the object on its own
    // destruction.  Since the athena framework will keep the services
    // around until the end of the job, that ought to be safe if used
    // as advertised.
    service = std::shared_ptr<T> (std::shared_ptr<void>(), &*serviceHandle);

    ANA_MSG_DEBUG ("Created component of type " << type());
    return StatusCode::SUCCESS;
  }

  template<typename T> ::StatusCode AsgServiceConfig ::
  makeService (ServiceHandle<T>& service) const
  {
    using namespace msgComponentConfig;

    ANA_CHECK (configureComponentExpert ("", false));
    service.setTypeAndName (typeAndName());
    ANA_CHECK (service.retrieve());
    ANA_CHECK (dynamic_cast<::AthService*>(&*service)->sysInitialize());

    ANA_MSG_DEBUG ("Created component of type " << type());
    return StatusCode::SUCCESS;
  }

#endif

}

#endif
