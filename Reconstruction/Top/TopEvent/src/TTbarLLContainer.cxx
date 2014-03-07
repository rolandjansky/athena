/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLLContainer.h"

template class TopRec::TTbarLLContainer<ElectronContainer, ElectronContainer>;
template class TopRec::TTbarLLContainer<ElectronContainer, Analysis::MuonContainer>;
template class TopRec::TTbarLLContainer<ElectronContainer, Analysis::TauJetContainer>;
template class TopRec::TTbarLLContainer<ElectronContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLContainer<Analysis::MuonContainer, Analysis::MuonContainer>;
template class TopRec::TTbarLLContainer<Analysis::MuonContainer, Analysis::TauJetContainer>;
template class TopRec::TTbarLLContainer<Analysis::MuonContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLContainer<Analysis::TauJetContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLContainer<TruthParticleContainer,TruthParticleContainer>;
