// this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSelectorTools/JetCleaningTool.h"
#include "CxxUtils/make_unique.h"
#include <memory>

namespace  {
  std::unique_ptr<JetCleaningTool> veryLooseBadTool;
  std::unique_ptr<JetCleaningTool> looseBadTool;
  std::unique_ptr<JetCleaningTool> mediumBadTool;
  std::unique_ptr<JetCleaningTool> tightBadTool;

  StatusCode initJetSelector (std::unique_ptr<JetCleaningTool>& ptr,
                              JetCleaningTool::CleaningLevel level)
  {
    ptr = CxxUtils::make_unique<JetCleaningTool> (level);
    return ptr->initialize();
  }

  StatusCode initJetSelectors(){
    static bool inited = false;
    if (inited) return StatusCode::SUCCESS;
    inited = true;

    if (initJetSelector (veryLooseBadTool, JetCleaningTool::VeryLooseBad).isFailure())
      return StatusCode::FAILURE;
    if (initJetSelector (looseBadTool,     JetCleaningTool::LooseBad).isFailure())
      return StatusCode::FAILURE;
    if (initJetSelector (mediumBadTool,    JetCleaningTool::MediumBad).isFailure())
      return StatusCode::FAILURE;
    if (initJetSelector (tightBadTool,     JetCleaningTool::TightBad).isFailure())
      return StatusCode::FAILURE;

    return StatusCode::SUCCESS;

  }
}

