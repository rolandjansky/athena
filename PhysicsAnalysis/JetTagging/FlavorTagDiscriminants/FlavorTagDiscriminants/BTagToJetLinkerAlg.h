/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_TO_JET_LINKER_ALG_H
#define BTAG_TO_JET_LINKER_ALG_H

#include "FlavorTagDiscriminants/BacklinkAlg.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODJet/JetContainer.h"

namespace detail {
  using BTagToJet_t = FlavorTagDiscriminants::BacklinkAlg<
    xAOD::JetContainer,
    xAOD::BTaggingContainer>;
}

// We seem to need a derived class to force Athena to build this.
class BTagToJetLinkerAlg: public detail::BTagToJet_t
{
public:
  BTagToJetLinkerAlg(const std::string& name, ISvcLocator* svcloc);
};

#endif
