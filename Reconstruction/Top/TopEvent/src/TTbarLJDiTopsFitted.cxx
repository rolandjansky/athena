/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLJDiTopsFitted.h"

template class TopRec::TTbarLJDiTopsFitted<ElectronContainer>;
template class TopRec::TTbarLJDiTopsFitted<Analysis::MuonContainer>;
template class TopRec::TTbarLJDiTopsFitted<Analysis::TauJetContainer>;
template class TopRec::TTbarLJDiTopsFitted<TruthParticleContainer>;
template class TopRec::TTbarLJDiTopsFitted<JetCollection>;
