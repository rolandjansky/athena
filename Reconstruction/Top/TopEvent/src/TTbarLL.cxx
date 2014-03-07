/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLL.h"

template class TopRec::TTbarLL<ElectronContainer,ElectronContainer>;
template class TopRec::TTbarLL<ElectronContainer,Analysis::MuonContainer>;
template class TopRec::TTbarLL<ElectronContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLL<ElectronContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLL<Analysis::MuonContainer,Analysis::MuonContainer>;
template class TopRec::TTbarLL<Analysis::MuonContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLL<Analysis::MuonContainer,Rec::TrackParticleContainer>;
template class TopRec::TTbarLL<Analysis::TauJetContainer,Analysis::TauJetContainer>;
template class TopRec::TTbarLL<TruthParticleContainer,TruthParticleContainer>;
