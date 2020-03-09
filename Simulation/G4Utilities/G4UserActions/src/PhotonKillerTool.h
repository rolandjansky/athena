/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__PHOTONKILLERTOOL_H
#define G4USERACTIONS_G4UA__PHOTONKILLERTOOL_H

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "PhotonKiller.h"

namespace G4UA
{

  /// @brief Tool which manages the PhotonKiller user action.
  ///
  class PhotonKillerTool : public UserActionToolBase<PhotonKiller>
  {

    public:

      /// Standard constructor
      PhotonKillerTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

    protected:

      /// Create the action for the current thread
      virtual std::unique_ptr<PhotonKiller>
      makeAndFillAction(G4AtlasUserActions&) override final;

  }; // class PhotonKillerTool

} // namespace G4UA

#endif
