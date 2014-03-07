/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLJMetContainer.h"

template class TopRec::TTbarLJMetContainer<ElectronContainer>;
template class TopRec::TTbarLJMetContainer<Analysis::MuonContainer>;
template class TopRec::TTbarLJMetContainer<Analysis::TauJetContainer>;
template class TopRec::TTbarLJMetContainer<TruthParticleContainer>;
template class TopRec::TTbarLJMetContainer<JetCollection>;
