/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__PHOTONKILLERTOOL_H
#define G4USERACTIONS_G4UA__PHOTONKILLERTOOL_H

#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include  "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/PhotonKiller.h"

namespace G4UA
{

  /// @brief Tool which manages the PhotonKiller user action.
  ///
  class PhotonKillerTool : public ActionToolBase<PhotonKiller>,
                           public ISteppingActionTool, public IPreTrackingActionTool
  {

    public:

      /// Standard constructor
      PhotonKillerTool(const std::string& type, const std::string& name,
                       const IInterface* parent);

      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

      virtual IPreTrackingAction* getPreTrackingAction() override final
      { return static_cast<IPreTrackingAction*>( getAction() ); }

    protected:

      virtual std::unique_ptr<PhotonKiller> makeAction() override final;

  }; // class PhotonKillerTool

} // namespace G4UA

#endif
