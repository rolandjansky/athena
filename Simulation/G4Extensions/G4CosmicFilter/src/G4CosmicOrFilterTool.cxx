/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicOrFilterTool.h"

namespace G4UA
{

  G4CosmicOrFilterTool::G4CosmicOrFilterTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
    : UserActionToolBase<G4CosmicOrFilter>(type, name, parent)
  {
    declareProperty("CollectionName", m_config.collectionName);
    declareProperty("CollectionName2", m_config.collectionName2);
    declareProperty("CollectionName3", m_config.collectionName3);
  }

  std::unique_ptr<G4CosmicOrFilter>
  G4CosmicOrFilterTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a G4CosmicFilter");
    auto action = std::make_unique<G4CosmicOrFilter>(m_config);
    actionList.eventActions.push_back( action.get() );
    return action;
  }

  StatusCode G4CosmicOrFilterTool::finalize()
  {
    // Accumulate results across threads
    G4CosmicOrFilter::Report report;
    m_actions.accumulate(report, &G4CosmicOrFilter::getReport,
                         &G4CosmicOrFilter::Report::merge);
    ATH_MSG_INFO("processed " << report.ntot << " events, " <<
                 report.npass << " events passed filter");

    return StatusCode::SUCCESS;
  }

} // namespace G4UA
