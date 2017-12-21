/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASALG_ATLASSTACKINGACTIONTOOL_H
#define G4ATLASALG_ATLASSTACKINGACTIONTOOL_H

// STL includes
#include <string>

// G4Atlas includes
#include "G4AtlasInterfaces/IUserActionTool.h"
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "AthenaStackingAction.h"


namespace G4UA
{

  /// @class AthenaStackingActionTool
  /// @brief Tool which manages the AthenaStackingAction
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class AthenaStackingActionTool : public UserActionToolBase<AthenaStackingAction>
  {

    public:

      /// Standard constructor
      AthenaStackingActionTool(const std::string& type, const std::string& name,
                               const IInterface* parent);

      /// Initialize the tool
      virtual StatusCode initialize() override final;

    protected:

      // Setup the user action for current thread
      virtual std::unique_ptr<AthenaStackingAction>
      makeAndFillAction(G4AtlasUserActions& actionLists) override final;

    private:

      /// Configuration parameters
      AthenaStackingAction::Config m_config;

  }; // class AthenaStackingActionTool

} // namespace G4UA

#endif
