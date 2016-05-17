/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "VerboseSelectorTool.h"

namespace G4UA{ 

  VerboseSelectorTool::VerboseSelectorTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<VerboseSelector>(type, name, parent), m_config(){

    declareProperty("TargetEvent",m_config.targetEvent);
    declareProperty("TargetTrack",m_config.targetTrack);
    //declareProperty("TargetBarcode",m_config.targetBarcode);
    declareProperty("VerboseLevel",m_config.verboseLevel);
    declareProperty("Continue",m_config.verb);
    // properties for area check at step time
    declareProperty("Xmin",m_config.Xmin);
    declareProperty("Xmax",m_config.Xmax);
    
    declareProperty("Ymin",m_config.Ymin);
    declareProperty("Ymax",m_config.Ymax);
    
    declareProperty("Zmin",m_config.Zmin);
    declareProperty("Zmax",m_config.Zmax);
    
  }
  
  std::unique_ptr<VerboseSelector>  VerboseSelectorTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<VerboseSelector>(m_config);
    return std::move(action);
  }
  
  StatusCode VerboseSelectorTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == ISteppingActionTool::interfaceID()) {
      *ppvIf = (ISteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IPreTrackingActionTool::interfaceID()) {
      *ppvIf = (IPreTrackingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IPostTrackingActionTool::interfaceID()) {
      *ppvIf = (IPostTrackingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IBeginEventActionTool::interfaceID()) {
      *ppvIf = (IBeginEventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    
    return ActionToolBase<VerboseSelector>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
