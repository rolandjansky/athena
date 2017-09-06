/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/GoodCaloSelector.h"
#include "TopEvent/EventTools.h"

namespace top {

  GoodCaloSelector::GoodCaloSelector()
  {  
  }

  bool GoodCaloSelector::apply(const top::Event& event) const 
  {
    if (event.m_info->isAvailable<char>("AnalysisTop_GOODCALO")) {
      if (event.m_info->auxdataConst<char>("AnalysisTop_GOODCALO") == 1) {
        return true;
      }
    }
    return false;
  }

  std::string GoodCaloSelector::name() const 
  {
    return "GOODCALO";
  }

}
