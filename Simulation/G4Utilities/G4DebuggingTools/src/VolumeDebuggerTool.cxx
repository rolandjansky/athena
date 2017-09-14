/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "VolumeDebuggerTool.h"
namespace G4UA{ 


  VolumeDebuggerTool::VolumeDebuggerTool(const std::string& type, const std::string& name,const IInterface* parent):
    ActionToolBase<VolumeDebugger>(type, name, parent), m_config(){

    declareProperty("OutputPath",m_config.path);
    declareProperty("TargetVolume",m_config.targetVolume);
    declareProperty("Resolution",m_config.res);
    declareProperty("Tolerance",m_config.tol);
    declareProperty("Verbose",m_config.verbose);
    declareProperty("RunGeoTest",m_config.volumeCheck);
    declareProperty("MaxCopiesToCheck", m_config.targetMaxCopiesToCheck);
    declareProperty("DumpGDML", m_config.dumpGDML);
    
  }

  std::unique_ptr<VolumeDebugger>  VolumeDebuggerTool::makeAction(){
    ATH_MSG_DEBUG("makeAction");
    auto action = CxxUtils::make_unique<VolumeDebugger>(m_config);
    return std::move(action);
  }
  StatusCode VolumeDebuggerTool::queryInterface(const InterfaceID& riid, void** ppvIf){
    
    if(riid == IG4RunActionTool::interfaceID()) {
      *ppvIf = (IG4RunActionTool*) this;
      addRef();
      return StatusCode::SUCCESS;
    }
    return ActionToolBase<VolumeDebugger>::queryInterface(riid, ppvIf);
  }
  
} // namespace G4UA 
