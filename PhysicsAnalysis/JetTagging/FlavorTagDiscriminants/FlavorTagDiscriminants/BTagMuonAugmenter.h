// for text editors: this file is -*- C++ -*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAG_MUON_AUGMENTER_H
#define BTAG_MUON_AUGMENTER_H

#include "xAODJet/Jet.h"

namespace FlavorTagDiscriminants {

  class BTagMuonAugmenter
  {
  public:
    BTagMuonAugmenter();
    ~BTagMuonAugmenter();
    BTagMuonAugmenter(BTagMuonAugmenter&&);
    void augment(const xAOD::Jet& jet) const;
  private:
    // You'll probably have to add some accessors here
  };

}
#endif
