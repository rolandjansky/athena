/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLLJetsContainer.h"

template class TopRec::TTbarLLJetsContainer<ElectronContainer, ElectronContainer>;
template class TopRec::TTbarLLJetsContainer<ElectronContainer, Analysis::MuonContainer>;
template class TopRec::TTbarLLJetsContainer<ElectronContainer, Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsContainer<ElectronContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLJetsContainer<Analysis::MuonContainer, Analysis::MuonContainer>;
template class TopRec::TTbarLLJetsContainer<Analysis::MuonContainer, Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsContainer<Analysis::MuonContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLJetsContainer<Analysis::TauJetContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsContainer<TruthParticleContainer,TruthParticleContainer>;
