/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FlavorTagDiscriminants/BTagMuonAugmenter.h"

namespace FlavorTagDiscriminants {
  BTagMuonAugmenter::BTagMuonAugmenter()
  {
    // you probably have to initialize something here
  }
  BTagMuonAugmenter::~BTagMuonAugmenter() = default;
  BTagMuonAugmenter::BTagMuonAugmenter(BTagMuonAugmenter&&) = default;
  void BTagMuonAugmenter::augment(const xAOD::Jet& ) const {
    // we probably have to do some things here
  }

}
