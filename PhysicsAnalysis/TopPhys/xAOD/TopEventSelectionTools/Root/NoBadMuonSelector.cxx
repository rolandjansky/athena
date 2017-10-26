/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NoBadMuonSelector.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"

namespace top {

  NoBadMuonSelector::NoBadMuonSelector(std::shared_ptr<top::TopConfig> config):
    m_config(config)
  {
    if (!m_config->isTruthDxAOD()) {
      m_muonSelectionTool = ToolHandle<CP::IMuonSelectionTool>("CP::MuonSelectionTool");
      top::check( m_muonSelectionTool.retrieve() , "Failed to retrieve muonSelectionTool" );
    }
  }

  bool NoBadMuonSelector::apply(const top::Event& event) const 
  {
    if (m_config->isTruthDxAOD()) return true;

    for (const auto* const muPtr : event.m_muons) {
      if (m_muonSelectionTool->isBadMuon(*muPtr)) {
	event.m_info->auxdecor<char>("AnalysisTop_HASBADMUON") = 1 ;
	return false;
      }
    }
    return true;
  }

  std::string NoBadMuonSelector::name() const 
  {
    return "NOBADMUON";
  }

}
