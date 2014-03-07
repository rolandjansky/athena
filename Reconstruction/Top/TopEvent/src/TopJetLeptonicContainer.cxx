/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TopJetLeptonicContainer.h"

template class TopRec::TopJetLeptonicContainer<ElectronContainer>;
template class TopRec::TopJetLeptonicContainer<Analysis::MuonContainer>;
template class TopRec::TopJetLeptonicContainer<Analysis::TauJetContainer>;
template class TopRec::TopJetLeptonicContainer<TruthParticleContainer>;	    
