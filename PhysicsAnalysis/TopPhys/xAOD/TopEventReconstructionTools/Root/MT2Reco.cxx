/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/MT2Reco.h"

#include "TopEvent/Event.h"

#ifdef ROOTCORE_PACKAGE_Oxbridgekinetics

namespace top {

MT2Reco::MT2Reco() {
}

MT2Reco::~MT2Reco() {
}

bool MT2Reco::apply(const top::Event& /*event*/) const {
    return true;
}

}

#endif //ROOTCORE_PACKAGE_Oxbridgekinetics
