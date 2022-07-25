/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef I_TRIGBTAGEMULATIONTOOL_H
#define I_TRIGBTAGEMULATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigBtagEmulationTool/EmulContext.h"
#include <string>

namespace Trig {

  class ITrigBtagEmulationTool 
    : virtual public IAlgTool {
  public:
    virtual const EmulContext& populateJetManagersTriggerObjects() const = 0;
    virtual bool isPassed(const std::string& chain) const = 0;
    virtual bool isPassed(const std::string& chain, const EmulContext&) const = 0;
  };

}

#endif
