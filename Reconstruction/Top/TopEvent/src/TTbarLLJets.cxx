/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLLJets.h"

template class TopRec::TTbarLLJets<ElectronContainer,ElectronContainer>;
template class TopRec::TTbarLLJets<ElectronContainer,Analysis::MuonContainer>;
template class TopRec::TTbarLLJets<ElectronContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLJets<ElectronContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLJets<Analysis::MuonContainer,Analysis::MuonContainer>;
template class TopRec::TTbarLLJets<Analysis::MuonContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLJets<Analysis::MuonContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLLJets<Analysis::TauJetContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLLJets<TruthParticleContainer,TruthParticleContainer>;
