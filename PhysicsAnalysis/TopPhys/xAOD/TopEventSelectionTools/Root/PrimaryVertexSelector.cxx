/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/PrimaryVertexSelector.h"
#include "TopEvent/EventTools.h"

namespace top {

  PrimaryVertexSelector::PrimaryVertexSelector()
  {  
  }

  bool PrimaryVertexSelector::apply(const top::Event& event) const 
  {
    if (event.m_info->isAvailable<char>("AnalysisTop_PRIVTX")) {
      if (event.m_info->auxdataConst<char>("AnalysisTop_PRIVTX") == 1) {
        return true;
      }
    }
    return false;
  }

  std::string PrimaryVertexSelector::name() const 
  {
    return "PRIVTX";
  }

}
