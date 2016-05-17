/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__HYPERSPACECATCHERTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__HYPERSPACECATCHERTOOL_H 

#include "G4AtlasInterfaces/IBeginRunActionTool.h"
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "HyperspaceCatcher.h"

namespace G4UA{ 
  
  class HyperspaceCatcherTool: 
  public ActionToolBase<HyperspaceCatcher>,
    public  IBeginRunActionTool,  public ISteppingActionTool
  {
  public:
    HyperspaceCatcherTool(const std::string& type, const std::string& name,const IInterface* parent);
    virtual IBeginRunAction* getBeginRunAction() override final 
    { return static_cast<IBeginRunAction*>( getAction() ); }
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    virtual std::unique_ptr<HyperspaceCatcher> makeAction() override final;
  private:
    HyperspaceCatcher::Config m_config;
  }; // class HyperspaceCatcherTool
} // namespace G4UA 
#endif
