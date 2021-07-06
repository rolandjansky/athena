/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author David Adams <dladams@bnl.gov> (for original implementation for tools)

#ifndef ASGSERVICES_ASGSERVICE_H
#define ASGSERVICES_ASGSERVICE_H

#include "AsgServices/IAsgService.h"

#ifdef XAOD_STANDALONE
#include "AsgMessaging/AsgMessaging.h"
#include "AsgTools/AsgComponent.h"
#else
#include "AthenaBaseComps/AthService.h"
#endif

#include <vector>
#include <utility>

class ISvcLocator;

namespace asg
{
  // Declare the type name of AsgService's base class
#ifndef XAOD_STANDALONE
  typedef ::AthService AsgServiceBase;
#else
  typedef AsgComponent AsgServiceBase;
#endif

  /// Base class for the dual-use service implementation classes
  ///
  /// This class can be used like AthService can be used for
  /// Athena-only services.
  ///
  /// Loosely based on the \ref AsgTool implementation.

  class AsgService : public virtual IAsgService,
                     public AsgServiceBase
  {
  public:
    AsgService (const std::string& name,
                ISvcLocator* pSvcLocator);


    /// set up/tear down functions
    /// \{
    virtual StatusCode initialize ();
    virtual StatusCode finalize ();
    /// \}

    /// Print the state of the service
    virtual void print() const;

    /// add the given interface to the list of interfaces
    template<typename T> void declareServiceInterface ();


#ifndef XAOD_STANDALONE
    /// query interface for gaudi
    virtual StatusCode queryInterface (const InterfaceID& riid, void **ppvi);

    /// list of interfaces we have
  private:
    std::vector<std::pair<const InterfaceID& (*)(),void *(*)(AsgService*)>> m_interfaces;
#endif

  }; // class AsgService



  template<typename T>
  void AsgService :: declareServiceInterface ()
  {
#ifndef XAOD_STANDALONE
    m_interfaces.emplace_back (T::interfaceID, [] (AsgService *self) -> void* {return dynamic_cast<T*>(self);});
#endif
  }

} // namespace asg

#endif // ASGSERVICES_ASGSERVICE_H
