/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLLJetsMet.h"

template class TopRec::TTbarLLJetsMet<ElectronContainer,ElectronContainer>;
template class TopRec::TTbarLLJetsMet<ElectronContainer,Analysis::MuonContainer>;
template class TopRec::TTbarLLJetsMet<ElectronContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsMet<ElectronContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLJetsMet<Analysis::MuonContainer,Analysis::MuonContainer>;
template class TopRec::TTbarLLJetsMet<Analysis::MuonContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsMet<Analysis::MuonContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLJetsMet<Analysis::TauJetContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLJetsMet<TruthParticleContainer,TruthParticleContainer>;
