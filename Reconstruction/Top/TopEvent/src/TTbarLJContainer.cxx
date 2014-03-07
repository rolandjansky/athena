/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLJContainer.h"

template class TopRec::TTbarLJContainer<ElectronContainer>;
template class TopRec::TTbarLJContainer<Analysis::MuonContainer>;
template class TopRec::TTbarLJContainer<Analysis::TauJetContainer>;
template class TopRec::TTbarLJContainer<TruthParticleContainer>;
template class TopRec::TTbarLJContainer<JetCollection>;
