/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4USERACTIONS_G4UA__RADLENGTHACTIONTOOL_H
#define G4USERACTIONS_G4UA__RADLENGTHACTIONTOOL_H
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4UserActions/RadLengthAction.h"


namespace G4UA{

  /// @class RadLengthActionTool
  /// @brief A tool to manage RadLengthAction actions
  ///
  /// creates one RadLengthAction instance per thread

  class RadLengthActionTool: public ActionToolBase<RadLengthAction>,
                             public IG4RunActionTool, public IG4EventActionTool,
                             public IG4SteppingActionTool
  {

  public:

    /// standard tool ctor
    RadLengthActionTool(const std::string& type, const std::string& name,const IInterface* parent);
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

    /// creates the action instances
    virtual std::unique_ptr<RadLengthAction> makeAction() override final;

  private:

    /// holds the python configuration
    RadLengthAction::Config m_config;

  }; // class RadLengthActionTool

} // namespace G4UA
#endif
