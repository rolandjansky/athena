/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/NoBadMuonSelector.h"
#include "TopEvent/EventTools.h"

namespace top {

  NoBadMuonSelector::NoBadMuonSelector() :
    m_muonSelectionTool("CP::MuonSelectionTool")
  {
    top::check( m_muonSelectionTool.retrieve() , "Failed to retrieve muonSelectionTool" );
  }

  bool NoBadMuonSelector::apply(const top::Event& event) const 
  {
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
