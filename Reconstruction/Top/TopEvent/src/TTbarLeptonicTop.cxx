/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLeptonicTop.h"

template class TopRec::TTbarLeptonicTop<ElectronContainer>;
template class TopRec::TTbarLeptonicTop<Analysis::MuonContainer>;
template class TopRec::TTbarLeptonicTop<Analysis::TauJetContainer>;
template class TopRec::TTbarLeptonicTop<TruthParticleContainer>;
