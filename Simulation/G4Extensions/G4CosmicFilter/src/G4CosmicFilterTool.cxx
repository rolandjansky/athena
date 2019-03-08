/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "G4CosmicFilter/G4CosmicFilterTool.h"

namespace G4UA
{

  G4CosmicFilterTool::G4CosmicFilterTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
    : UserActionToolBase<G4CosmicFilter>(type, name, parent)
  {
    declareProperty("CollectionName", m_config.collectionName);
    declareProperty("PDGId", m_config.PDGId);
    declareProperty("PtMin", m_config.ptMin);
    declareProperty("PtMax", m_config.ptMax);
  }

  std::unique_ptr<G4CosmicFilter>
  G4CosmicFilterTool::makeAndFillAction(G4AtlasUserActions& actionList)
  {
    ATH_MSG_DEBUG("Constructing a G4CosmicFilter");
    auto action = std::make_unique<G4CosmicFilter>(m_config);
    actionList.eventActions.push_back( action.get() );
    return action;
  }

  StatusCode G4CosmicFilterTool::finalize()
  {
    // Accumulate results across threads
    G4CosmicFilter::Report report;
    m_actions.accumulate(report, &G4CosmicFilter::getReport,
                         &G4CosmicFilter::Report::merge);
    ATH_MSG_INFO("processed " << report.ntot << " events, " <<
                 report.npass << " events passed filter");

    return StatusCode::SUCCESS;
  }

} // namespace G4UA
