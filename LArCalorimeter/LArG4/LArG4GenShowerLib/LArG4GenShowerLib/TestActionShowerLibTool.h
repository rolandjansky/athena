/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4GENSHOWERLIB_G4UA__TESTACTIONSHOWERLIBTOOL_H 
#define LARG4GENSHOWERLIB_G4UA__TESTACTIONSHOWERLIBTOOL_H 
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/IEndRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
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
    public IBeginEventActionTool,  public IEndEventActionTool,  public IBeginRunActionTool,  public IEndRunActionTool,  public ISteppingActionTool
  {
    
  public:
    /// standard tool ctor
    TestActionShowerLibTool(const std::string& type, const std::string& name,const IInterface* parent);
    /// retrieves the BoE action
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    /// retrieves the EoE action
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    /// retrieves the BoR action
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    /// retrieves the EoR action
    virtual IEndRunAction* getEndRunAction() override final 
    { return static_cast<IEndRunAction*>( getAction() ); }
    /// retrieves the stepping action
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    /// Gaudi interface handling
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    /// creates the action
    virtual std::unique_ptr<TestActionShowerLib> makeAction() override final;
  private:
  }; // class TestActionShowerLibTool
  

} // namespace G4UA 
#endif
