/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef I_TRIGBTAGEMULATIONTOOL_H
#define I_TRIGBTAGEMULATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <string>

namespace Trig {

  class ITrigBtagEmulationTool 
    : virtual public IAlgTool {
  public:
    virtual StatusCode populateJetManagersTriggerObjects() = 0;
    virtual bool isPassed(const std::string& chain) const = 0;
  };

}

#endif
