/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4ATLASINTERFACES_G4UA__IUSERACTIONSVC_H
#define G4ATLASINTERFACES_G4UA__IUSERACTIONSVC_H

// Framework includes
#include "GaudiKernel/IService.h"

namespace G4UA
{

  /// @class IUserActionSvc
  /// @brief Abstract interface for the UserActionSvc
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class IUserActionSvc : virtual public IService
  {

  public:

    /// @brief Initialize all the user actions for the current thread.
    virtual StatusCode initializeActions() = 0;

    /// Interface declaration
    static const InterfaceID& interfaceID()
    {
      static const InterfaceID iid_IUASvc("G4UA::IUserActionSvc", 1, 0);
      return iid_IUASvc;
    }

  }; // class IUserActionSvc

} // namespace G4UA

#endif
