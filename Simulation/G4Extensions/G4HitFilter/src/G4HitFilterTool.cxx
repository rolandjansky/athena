/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4HitFilterTool.h"


namespace G4UA
{

  //----------------------------------------------------------------------------
  G4HitFilterTool::G4HitFilterTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
    : UserActionToolBase<G4HitFilter>(type, name, parent)
  {
    declareProperty("VolumeNames", m_config.volumenames);
  }

  //----------------------------------------------------------------------------
  std::unique_ptr<G4HitFilter>
  G4HitFilterTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a G4HitFilter action");
    auto action = std::make_unique<G4HitFilter>(m_config);
    actionList.runActions.push_back( action.get() );
    actionList.eventActions.push_back( action.get() );
    return action;
  }

  //----------------------------------------------------------------------------
  StatusCode G4HitFilterTool::finalize()
  {
    // Merge results across worker threads
    G4HitFilter::Report report;
    m_actions.accumulate(report, &G4HitFilter::getReport,
                         &G4HitFilter::Report::merge);

    // Report the results
    ATH_MSG_INFO("processed " << report.ntot << " events, " <<
                 report.npass << " events passed filter ");
    return StatusCode::SUCCESS;
  }

} // namespace G4UA
