/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__FLUXRECORDERTOOL_H
#define G4USERACTIONS_G4UA__FLUXRECORDERTOOL_H

#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/FluxRecorder.h"

namespace G4UA
{

  /// 	
  /// @class FluxRecorderTool
  /// @brief Tool which manages the FluxRecorder action.
  ///
  /// Create the FluxRecorder  for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class FluxRecorderTool : public ActionToolBase<FluxRecorder>,
                           public IG4RunActionTool, public IG4EventActionTool,
                           public IG4SteppingActionTool
  {

    public:

      /// standard tool ctor
      FluxRecorderTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// retrieves the run action
      virtual G4UserRunAction* getRunAction() override final
      { return static_cast<G4UserRunAction*>( getAction() ); }
      /// retrieves the event action
      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }
      /// retrieves the stepping action
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }

    protected:

      /// create action for this thread
      virtual std::unique_ptr<FluxRecorder> makeAction() override final;

  }; // class FluxRecorderTool

} // namespace G4UA
#endif
