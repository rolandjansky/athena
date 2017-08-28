/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA_COSMICPERIGEEACTIONTOOL_H
#define G4USERACTIONS_G4UA_COSMICPERIGEEACTIONTOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"

// Local includes
#include "G4UserActions/CosmicPerigeeAction.h"

namespace G4UA
{

  /// @class CosmicPerigeeActionTool
  /// @brief Tool which manages the CosmicPerigeeAction action.
  ///
  /// Create the CosmicPerigeeAction for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class CosmicPerigeeActionTool : public ActionToolBase<CosmicPerigeeAction>,
                                  public ISteppingActionTool,
                                  public IEndEventActionTool,
                                  public IBeginEventActionTool,
                                  public IPreTrackingActionTool
  {

    public:

      /// Standard constructor
      CosmicPerigeeActionTool(const std::string& type, const std::string& name,
		              const IInterface* parent);


      /// Retrieve the stepping action interface
      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }

      /// Retrieve the begin-event action interface
      virtual IBeginEventAction* getBeginEventAction() override final
      { return static_cast<IBeginEventAction*>( getAction() ); }

      /// Retrieve the end-event action interface
      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }

      /// Retrieve the preTracking action interface
      virtual IPreTrackingAction* getPreTrackingAction() override final
      { return static_cast<IPreTrackingAction*>( getAction() ); }

    protected:

      /// Create action for this thread
      virtual std::unique_ptr<CosmicPerigeeAction> makeAction() override final;

    private:

      /// Configuration parameters
      CosmicPerigeeAction::Config m_config;

  }; // class CosmicPerigeeActionTool

} // namespace G4UA

#endif
