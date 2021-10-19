/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagToJetLinkerAlg.h"

BTagToJetLinkerAlg::BTagToJetLinkerAlg(
  const std::string& name,
  ISvcLocator* svcloc):
  detail::BTagToJet_t(name, svcloc)
{}
