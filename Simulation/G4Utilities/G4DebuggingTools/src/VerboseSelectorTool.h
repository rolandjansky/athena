/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__VERBOSESELECTORTOOL_H 
#define G4DEBUGGINGTOOLS_G4UA__VERBOSESELECTORTOOL_H 

#include "G4AtlasInterfaces/ISteppingActionTool.h"
#include "G4AtlasInterfaces/IPreTrackingActionTool.h"
#include "G4AtlasInterfaces/IPostTrackingActionTool.h"
#include "G4AtlasInterfaces/IBeginEventActionTool.h"
#include "G4AtlasTools/ActionToolBase.h"
#include "VerboseSelector.h"

namespace G4UA{ 

  class VerboseSelectorTool: 
  public ActionToolBase<VerboseSelector>, public IBeginEventActionTool,
    public ISteppingActionTool,  public IPreTrackingActionTool,  public IPostTrackingActionTool
  {
    
  public:
    VerboseSelectorTool(const std::string& type, const std::string& name,const IInterface* parent);
    virtual ISteppingAction* getSteppingAction() override final 
    { return static_cast<ISteppingAction*>( getAction() ); }
    virtual IPreTrackingAction* getPreTrackingAction() override final 
    { return static_cast<IPreTrackingAction*>( getAction() ); }
    virtual IPostTrackingAction* getPostTrackingAction() override final 
    { return static_cast<IPostTrackingAction*>( getAction() ); }
    virtual IBeginEventAction* getBeginEventAction() override final 
    { return static_cast<IBeginEventAction*>( getAction() ); }

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override;
  protected:
    virtual std::unique_ptr<VerboseSelector> makeAction() override final;
  private:
    VerboseSelector::Config m_config;
  }; // class VerboseSelectorTool
  
  
} // namespace G4UA 
#endif
