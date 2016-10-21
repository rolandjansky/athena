/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKG4USERACTIONS_G4UA__GEANTFOLLOWERTOOL_H 
#define TRKG4USERACTIONS_G4UA__GEANTFOLLOWERTOOL_H 
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "TrkG4UserActions/GeantFollower.h"

namespace G4UA{ 

  /// @class AthenaStackingActionTool
  /// @brief Tool which manages the GeantFollower action
  ///
  /// @author Andrea Di Simone
  ///
  
  class GeantFollowerTool: 
  public ActionToolBase<GeantFollower>,
    public IBeginEventActionTool,  public IEndEventActionTool,  public IBeginRunActionTool,  public ISteppingActionTool
  {
    
  public:
    /// Standard constructor
    GeantFollowerTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// Retrieve the BoE action
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// Retrieve the EoE action
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// Retrieve the BoR action
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// Retrieve the stepping action
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
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
