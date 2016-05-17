/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__G4ATLANTISDUMPERTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__G4ATLANTISDUMPERTOOL_H 
#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IEndEventActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "G4AtlantisDumper.h"

namespace G4UA{ 

  class G4AtlantisDumperTool: 
  public ActionToolBase<G4AtlantisDumper>,
    public ISteppingActionTool,  public IEndEventActionTool,  public IBeginEventActionTool
  {
    
  public:
    G4AtlantisDumperTool(const std::string& type, const std::string& name,const IInterface* parent);
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    virtual IEndEventAction* getEndEventAction() override final 
    { return static_cast<IEndEventAction*>( getAction() ); }
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    virtual std::unique_ptr<G4AtlantisDumper> makeAction() override final;
  private:
    G4AtlantisDumper::Config m_config;
  }; // class G4AtlantisDumperTool
  
  
} // namespace G4UA 
#endif
