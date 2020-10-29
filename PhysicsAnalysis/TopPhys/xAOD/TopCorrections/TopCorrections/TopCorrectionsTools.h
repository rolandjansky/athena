/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TOPTOPCORRECTIONTOOLS_H__
#define TOPTOPCORRECTIONTOOLS_H__

#include "TopConfiguration/TopConfig.h"
#include <string>

namespace top {
  // Functions useful for b-tagging systematic name mapping
  std::string bTagNamedSystCheck(std::shared_ptr<top::TopConfig>,
                                 const std::string&,
                                 const std::string&,
                                 const bool,
                                 const bool);
  std::string betterBtagNamedSyst(const std::string&);
}

#endif
