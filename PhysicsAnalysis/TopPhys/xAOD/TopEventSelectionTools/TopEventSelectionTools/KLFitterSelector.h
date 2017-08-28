/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: KLFitterSelector.h 665436 2015-05-06 19:10:23Z morrisj $
#ifndef ANALYSISTOP_TOPEVENTSELECTIONTOOLS_KLFITTERSELECTOR_H
#define ANALYSISTOP_TOPEVENTSELECTIONTOOLS_KLFITTERSELECTOR_H

#include "TopEventSelectionTools/SignValueSelector.h"

namespace top{
  
  /**
   * @brief Apply a cut on the KLFitter Results
   */
  
  // Forward declare
  class Event;
  
  class KLFitterSelector : public SignValueSelector {
    public:
      explicit KLFitterSelector(const std::string& params);
      bool apply(const top::Event&) const override;
  };
}
#endif
