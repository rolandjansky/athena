/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/JetToBTagLinkerAlg.h"

JetToBTagLinkerAlg::JetToBTagLinkerAlg(
  const std::string& name,
  ISvcLocator* svcloc):
  detail::JetToBTag_t(name, svcloc)
{}
