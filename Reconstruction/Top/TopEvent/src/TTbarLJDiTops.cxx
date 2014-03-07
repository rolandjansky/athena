/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLJDiTops.h"

template class TopRec::TTbarLJDiTops<ElectronContainer>;
template class TopRec::TTbarLJDiTops<Analysis::MuonContainer>;
template class TopRec::TTbarLJDiTops<Analysis::TauJetContainer>;
template class TopRec::TTbarLJDiTops<TruthParticleContainer>;
template class TopRec::TTbarLJDiTops<JetCollection>;
