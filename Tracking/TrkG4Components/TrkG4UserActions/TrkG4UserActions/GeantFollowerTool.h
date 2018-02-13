/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__GEANTFOLLOWERTOOL_H 
#define TRKG4USERACTIONS_G4UA__GEANTFOLLOWERTOOL_H 
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TrkG4UserActions/GeantFollower.h"

namespace G4UA{ 

  /// @class GeantFollowerTool
  /// @brief Tool which manages the GeantFollower action
  ///
  /// @author Andrea Di Simone
  ///
  
  class GeantFollowerTool: 
  public ActionToolBase<GeantFollower>,
    public IG4EventActionTool, public IG4RunActionTool,  public IG4SteppingActionTool
  {
    
  public:
    /// Standard constructor
    GeantFollowerTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Retrieve the event action
    virtual G4UserEventAction* getEventAction() override final 
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// Retrieve the run action
    virtual G4UserRunAction* getRunAction() override final 
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// Retrieve the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final 
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// Create an action for this thread
    virtual std::unique_ptr<GeantFollower> makeAction() override final;
  private:
    /// Configuration parameters
    GeantFollower::Config m_config;
  }; // class GeantFollowerTool
  
  
} // namespace G4UA 
#endif
