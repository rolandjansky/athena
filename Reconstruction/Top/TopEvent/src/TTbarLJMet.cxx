/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLJMet.h"

template class TopRec::TTbarLJMet<ElectronContainer>;
template class TopRec::TTbarLJMet<Analysis::MuonContainer>;
template class TopRec::TTbarLJMet<Analysis::TauJetContainer>;
template class TopRec::TTbarLJMet<TruthParticleContainer>;
template class TopRec::TTbarLJMet<JetCollection>;
