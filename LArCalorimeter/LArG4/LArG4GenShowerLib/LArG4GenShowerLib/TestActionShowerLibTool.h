/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4GENSHOWERLIB_G4UA__TESTACTIONSHOWERLIBTOOL_H 
#define LARG4GENSHOWERLIB_G4UA__TESTACTIONSHOWERLIBTOOL_H 
#include "G4AtlasInterfaces/IG4EventActionTool.h"
#include "G4AtlasInterfaces/IG4RunActionTool.h"
#include "G4AtlasInterfaces/IG4SteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "LArG4GenShowerLib/TestActionShowerLib.h"
namespace G4UA{ 

  /// @class TestActionShowerLibTool
  /// @brief A tool to manage TestActionShowerLib
  ///
  /// creates one instance of TestActionShowerLib per thread
  ///
  /// @author Andrea Di Simone
  
  class TestActionShowerLibTool: 
  public ActionToolBase<TestActionShowerLib>,
    public IG4EventActionTool, public IG4RunActionTool, public IG4SteppingActionTool
  {
    
  public:
    /// standard tool ctor
    TestActionShowerLibTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// retrieves the event action
    virtual G4UserEventAction* getEventAction() override final
    { return static_cast<G4UserEventAction*>( getAction() ); }
    /// retrieves the run action
    virtual G4UserRunAction* getRunAction() override final
    { return static_cast<G4UserRunAction*>( getAction() ); }
    /// retrieves the stepping action
    virtual G4UserSteppingAction* getSteppingAction() override final
    { return static_cast<G4UserSteppingAction*>( getAction() ); }
    /// Gaudi interface handling
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// creates the action
    virtual std::unique_ptr<TestActionShowerLib> makeAction() override final;
  private:
  }; // class TestActionShowerLibTool
  

} // namespace G4UA 
#endif
