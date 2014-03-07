/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLJDiTopsFittedContainer.h"

template class TopRec::TTbarLJDiTopsFittedContainer<ElectronContainer>;
template class TopRec::TTbarLJDiTopsFittedContainer<Analysis::MuonContainer>;
template class TopRec::TTbarLJDiTopsFittedContainer<Analysis::TauJetContainer>;
template class TopRec::TTbarLJDiTopsFittedContainer<TruthParticleContainer>;
template class TopRec::TTbarLJDiTopsFittedContainer<JetCollection>;
