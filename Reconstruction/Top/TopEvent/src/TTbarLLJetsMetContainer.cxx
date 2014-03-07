/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLLJetsMetContainer.h"

template class TopRec::TTbarLLJetsMetContainer<ElectronContainer, ElectronContainer>;
template class TopRec::TTbarLLJetsMetContainer<ElectronContainer, Analysis::MuonContainer>;
template class TopRec::TTbarLLJetsMetContainer<ElectronContainer, Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsMetContainer<ElectronContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLJetsMetContainer<Analysis::MuonContainer, Analysis::MuonContainer>;
template class TopRec::TTbarLLJetsMetContainer<Analysis::MuonContainer, Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsMetContainer<Analysis::MuonContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLJetsMetContainer<Analysis::TauJetContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsMetContainer<TruthParticleContainer,TruthParticleContainer>;
