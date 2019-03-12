/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicAndFilterTool.h"

namespace G4UA
{

  G4CosmicAndFilterTool::G4CosmicAndFilterTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent)
    : UserActionToolBase<G4CosmicAndFilter>(type, name, parent)
  {
    declareProperty("CollectionName", m_config.collectionName);
    declareProperty("CollectionName2", m_config.collectionName2);
  }

  std::unique_ptr<G4CosmicAndFilter>
  G4CosmicAndFilterTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a G4CosmicAndFilter");
    auto action = std::make_unique<G4CosmicAndFilter>(m_config);
    actionList.eventActions.push_back( action.get() );
    return action;
  }

  StatusCode G4CosmicAndFilterTool::finalize()
  {
    // Accumulate results across threads
    G4CosmicAndFilter::Report report;
    m_actions.accumulate(report, &G4CosmicAndFilter::getReport,
                         &G4CosmicAndFilter::Report::merge);
    ATH_MSG_INFO("processed " << report.ntot << " events, " <<
                 report.npass << " events passed filter");
    return StatusCode::SUCCESS;
  }

} // namespace G4UA
