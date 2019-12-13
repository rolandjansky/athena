/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkG4UserActions/EnergyLossRecorderTool.h"
#include "TrkValInterfaces/IPositionMomentumWriter.h"

namespace G4UA
{

  EnergyLossRecorderTool::EnergyLossRecorderTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
    : UserActionToolBase<EnergyLossRecorder>(type, name, parent),
      m_pmWriter("")
  {
    declareProperty("PositionMomentumWriter", m_pmWriter, "");
  }

  StatusCode EnergyLossRecorderTool::initialize()
  {
    if(!m_pmWriter.empty()) {
      ATH_CHECK(m_pmWriter.retrieve());
      // FIXME: thread-unsafe usage of a component in a thread-local action?
      // See ATLASSIM-3562.
      m_config.pmWriter = &(*m_pmWriter);
    }
    return StatusCode::SUCCESS;
  }

  std::unique_ptr<EnergyLossRecorder>
  EnergyLossRecorderTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing an EnergyLossRecorder action");
    auto action = std::make_unique<EnergyLossRecorder>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    actionList.steppingActions.push_back( action.get() );
    return action;
  }

} // namespace G4UA
