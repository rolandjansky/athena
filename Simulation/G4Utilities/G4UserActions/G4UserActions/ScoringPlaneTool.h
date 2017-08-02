/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__SCORINGPLANETOOL_H
#define G4USERACTIONS_G4UA__SCORINGPLANETOOL_H
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
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
                           public IBeginRunActionTool, public IEndRunActionTool,
                           public ISteppingActionTool, public IBeginEventActionTool,
                           public IEndEventActionTool
  {

    public:
      /// standard tool ctor
      ScoringPlaneTool(const std::string& type, const std::string& name,const IInterface* parent);
      /// retrieves BoR action
      virtual IBeginRunAction* getBeginRunAction() override final
      { return static_cast<IBeginRunAction*>( getAction() ); }
      /// retrieves EoR action
      virtual IEndRunAction* getEndRunAction() override final
      { return static_cast<IEndRunAction*>( getAction() ); }
      /// retrieves stepping action
      virtual ISteppingAction* getSteppingAction() override final
      { return static_cast<ISteppingAction*>( getAction() ); }
      /// retrieves BoE action
      virtual IBeginEventAction* getBeginEventAction() override final
      { return static_cast<IBeginEventAction*>( getAction() ); }
      /// retrieves EoE action
      virtual IEndEventAction* getEndEventAction() override final
      { return static_cast<IEndEventAction*>( getAction() ); }
    protected:
      /// creates the action instances
      virtual std::unique_ptr<ScoringPlane> makeAction() override final;
    private:
      /// holds the python configuration
      ScoringPlane::Config m_config;

  }; // class ScoringPlaneTool

} // namespace G4UA
#endif
