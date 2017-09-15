/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__SCORINGPLANETOOL_H
#define G4USERACTIONS_G4UA__SCORINGPLANETOOL_H
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/ScoringPlane.h"


namespace G4UA
{

  /// @class ScoringPlaneTool
  /// @brief A tool to manage ScoprinPlane actions
  ///
  /// creates one ScoprinPlane instance per thread
  /// @author Andrea Di Simone
  ///
  class ScoringPlaneTool : public ActionToolBase<ScoringPlane>,
                           public IG4RunActionTool,
                           public IG4SteppingActionTool,
                           public IG4EventActionTool
  {

    public:

      /// standard tool ctor
      ScoringPlaneTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// retrieves run action
      virtual G4UserRunAction* getRunAction() override final
      { return static_cast<G4UserRunAction*>( getAction() ); }
      /// retrieves stepping action
      virtual G4UserSteppingAction* getSteppingAction() override final
      { return static_cast<G4UserSteppingAction*>( getAction() ); }
      /// retrieves event action
      virtual G4UserEventAction* getEventAction() override final
      { return static_cast<G4UserEventAction*>( getAction() ); }

    protected:

      /// creates the action instances
      virtual std::unique_ptr<ScoringPlane> makeAction() override final;

    private:

      /// holds the python configuration
      ScoringPlane::Config m_config;

  }; // class ScoringPlaneTool

} // namespace G4UA
#endif
