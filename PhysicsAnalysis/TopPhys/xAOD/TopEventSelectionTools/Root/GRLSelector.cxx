/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/GRLSelector.h"

#include "TopEvent/EventTools.h"

#include "xAODEventInfo/EventInfo.h"

namespace top {

  GRLSelector::GRLSelector()
  {  
  }

  bool GRLSelector::apply(const top::Event& event) const 
  {
    if (event.m_info->isAvailable<char>("AnalysisTop_GRL")) {
      if (event.m_info->auxdataConst<char>("AnalysisTop_GRL") == 1) {
        return true;
      }
    }
    return false;
  }

  std::string GRLSelector::name() const 
  {
    return "GRL";
  }

}
