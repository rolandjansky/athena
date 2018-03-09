/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "G4AtlantisDumperTool.h"
namespace G4UA{ 


G4AtlantisDumperTool::G4AtlantisDumperTool
(const std::string& type, const std::string& name,const IInterface* parent):

  ActionToolBase<G4AtlantisDumper>(type, name, parent), m_config(){
 
  declareProperty("Edep",m_config.tedep_cut);
  declareProperty("Length",m_config.length_cut);
  declareProperty("P",m_config.p_cut);
  declareProperty("KE",m_config.ke_cut);

}
  std::unique_ptr<G4AtlantisDumper>  G4AtlantisDumperTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<G4AtlantisDumper>(m_config);
    return std::move(action);
  }
  StatusCode G4AtlantisDumperTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4SteppingActionTool::interfaceID()) {
      *ppvIf = (IG4SteppingActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } if(riid == IG4EventActionTool::interfaceID()) {
      *ppvIf = (IG4EventActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    } return ActionToolBase<G4AtlantisDumper>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
