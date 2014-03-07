/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLJDiTopsContainer.h"

template class TopRec::TTbarLJDiTopsContainer<ElectronContainer>;
template class TopRec::TTbarLJDiTopsContainer<Analysis::MuonContainer>;
template class TopRec::TTbarLJDiTopsContainer<Analysis::TauJetContainer>;
template class TopRec::TTbarLJDiTopsContainer<TruthParticleContainer>;
template class TopRec::TTbarLJDiTopsContainer<JetCollection>;
