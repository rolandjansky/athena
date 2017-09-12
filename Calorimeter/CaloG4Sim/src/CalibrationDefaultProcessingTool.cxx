/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloG4Sim/CalibrationDefaultProcessingTool.h"

namespace G4UA{ 
  
  namespace CaloG4{
    
    CalibrationDefaultProcessingTool::CalibrationDefaultProcessingTool(const std::string& type, const std::string& name,const IInterface* parent):
      
      ActionToolBase<CalibrationDefaultProcessing>(type, name, parent), m_config(){
      
      declareProperty("SDName", m_config.SDName);
      
    }
    
    std::unique_ptr<CalibrationDefaultProcessing>  CalibrationDefaultProcessingTool::makeAction(){
      ATH_MSG_DEBUG("makeAction");
      return std::make_unique<CalibrationDefaultProcessing>(m_config);
    }
    
    StatusCode CalibrationDefaultProcessingTool::queryInterface(const InterfaceID& riid, void** ppvIf){
      
      if(riid == IG4EventActionTool::interfaceID()) {
	*ppvIf = (IG4EventActionTool*) this;
	addRef();
	return StatusCode::SUCCESS;
      }
      if(riid == IG4SteppingActionTool::interfaceID()) {
	*ppvIf = (IG4SteppingActionTool*) this;
	addRef();
	return StatusCode::SUCCESS;
      }
      return ActionToolBase<CalibrationDefaultProcessing>::queryInterface(riid, ppvIf);
    }
    
  } // namespace CaloG4

} // namespace G4UA 
