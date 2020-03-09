/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VolumeDebuggerTool.h"

namespace G4UA
{

  VolumeDebuggerTool::VolumeDebuggerTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
    : UserActionToolBase<VolumeDebugger>(type, name, parent)
  {
    declareProperty("OutputPath", m_config.path);
    declareProperty("TargetVolume", m_config.targetVolume);
    declareProperty("Resolution", m_config.res);
    declareProperty("Tolerance", m_config.tol);
    declareProperty("Verbose", m_config.verbose);
    declareProperty("RunGeoTest", m_config.volumeCheck);
    declareProperty("MaxCopiesToCheck", m_config.targetMaxCopiesToCheck);
    declareProperty("DumpGDML", m_config.dumpGDML);
    declareProperty("DumpPhysicsRegions", m_config.dumpPhysicsRegions);
  }

  std::unique_ptr<VolumeDebugger>
  VolumeDebuggerTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a VolumeDebugger");
    auto action = std::make_unique<VolumeDebugger>(m_config);
    actionList.runActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
