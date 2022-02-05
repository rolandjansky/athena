/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/JetTagDecoratorAlg.h"

namespace FlavorTagDiscriminants {
  JetTagDecoratorAlg::JetTagDecoratorAlg(
    const std::string& name, ISvcLocator* svcloc):
    detail::JetTag_t(name, svcloc)
  {
  }
  StatusCode JetTagDecoratorAlg::initialize() {
    return initializeInternal();
  }
}
