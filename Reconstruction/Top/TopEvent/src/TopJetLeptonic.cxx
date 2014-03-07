/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TopJetLeptonic.h"

template class TopRec::TopJetLeptonic<ElectronContainer>;
template class TopRec::TopJetLeptonic<Analysis::MuonContainer>;
template class TopRec::TopJetLeptonic<Analysis::TauJetContainer>;
template class TopRec::TopJetLeptonic<TruthParticleContainer>;	    
