/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JET_TO_BTAG_LINKER_ALG_H
#define JET_TO_BTAG_LINKER_ALG_H

#include "FlavorTagDiscriminants/BacklinkAlg.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODJet/JetContainer.h"

namespace detail {
  using JetToBTag_t = FlavorTagDiscriminants::BacklinkAlg<
    xAOD::BTaggingContainer,
    xAOD::JetContainer
    >;
}

// We seem to need a derived class to force Athena to build this.
class JetToBTagLinkerAlg: public detail::JetToBTag_t
{
public:
  JetToBTagLinkerAlg(const std::string& name, ISvcLocator* svcloc);
};


#endif
