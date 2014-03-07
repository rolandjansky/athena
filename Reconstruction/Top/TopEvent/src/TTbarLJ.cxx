/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEvent/TTbarLJ.h"

template class TopRec::TTbarLJ<ElectronContainer>;
template class TopRec::TTbarLJ<Analysis::MuonContainer>;
template class TopRec::TTbarLJ<Analysis::TauJetContainer>;
template class TopRec::TTbarLJ<TruthParticleContainer>;	    
template class TopRec::TTbarLJ<JetCollection>;	    
