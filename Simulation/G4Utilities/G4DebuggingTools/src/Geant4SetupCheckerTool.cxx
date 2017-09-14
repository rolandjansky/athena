/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "Geant4SetupCheckerTool.h"

namespace G4UA{ 
  
  
  Geant4SetupCheckerTool::Geant4SetupCheckerTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<Geant4SetupChecker>(type, name, parent),
    m_file_location("reference_file.txt"),
    m_test(true){
    declareProperty("ReferenceFile",m_file_location);
    declareProperty("RunTest",m_test);
  }
  
  std::unique_ptr<Geant4SetupChecker>  Geant4SetupCheckerTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<Geant4SetupChecker>(m_file_location,m_test);
    return std::move(action);
  }

  StatusCode Geant4SetupCheckerTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    if(riid == IG4RunActionTool::interfaceID()) {
      *ppvIf = (IG4RunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<Geant4SetupChecker>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
