/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventTPCnv/TTbarJJContainer_p1.h"
#include "TopEventTPCnv/TTbarJJ_p1.h"
#include "TopEventTPCnv/TTbarJetsBase_p1.h"

#include "TopEventTPCnv/TTbarJJTopContainer_p1.h"
#include "TopEventTPCnv/TTbarJJTop_p1.h"

#include "TopEventTPCnv/TTbarLJContainer_p1.h"
#include "TopEventTPCnv/TTbarLJ_p1.h"

#include "TopEventTPCnv/TTbarLJMetContainer_p1.h"
#include "TopEventTPCnv/TTbarLJMet_p1.h"

#include "TopEventTPCnv/TTbarLJTopContainer_p1.h"
#include "TopEventTPCnv/TTbarLJTop_p1.h"

#include "TopEventTPCnv/TTbarLJDiTopsContainer_p1.h"
#include "TopEventTPCnv/TTbarLJDiTops_p1.h"

#include "TopEventTPCnv/TTbarLJDiTopsFittedContainer_p1.h"
#include "TopEventTPCnv/TTbarLJDiTopsFitted_p1.h"

#include "TopEventTPCnv/TTbarLLContainer_p1.h"
#include "TopEventTPCnv/TTbarLL_p1.h"

#include "TopEventTPCnv/TTbarLLJetsContainer_p1.h"
#include "TopEventTPCnv/TTbarLLJets_p1.h"

#include "TopEventTPCnv/TTbarLLJetsMetContainer_p1.h"
#include "TopEventTPCnv/TTbarLLJetsMet_p1.h"

#include "TopEventTPCnv/TopJetHadronicContainer_p1.h"
#include "TopEventTPCnv/TopJetHadronic_p1.h"
#include "TopEventTPCnv/TopJet_p1.h"

#include "TopEventTPCnv/TopJetLeptonicContainer_p1.h"
#include "TopEventTPCnv/TopJetLeptonic_p1.h"

//----------------------------------------------------------------
// Need to instantiate templates *here* and not in some .cxx file, or
// genreflex would not produce the dictionaries.

// NOTE: changed this, these are no longer templates so don't need 
// to instantiate them anymore

/* template class TTbarLJContainer_p1<ElectronContainer>; */
/* template class TTbarLJContainer_p1<Analysis::MuonContainer>; */
/* template class TTbarLJContainer_p1<Analysis::TauJetContainer>; */
/* template class TTbarLJContainer_p1<TruthParticleContainer>; */

/* template class TTbarLJTopContainer_p1<ElectronContainer>; */
/* template class TTbarLJTopContainer_p1<Analysis::MuonContainer>; */
/* template class TTbarLJTopContainer_p1<Analysis::TauJetContainer>; */
/* template class TTbarLJTopContainer_p1<TruthParticleContainer>; */

/* template class TTbarLLContainer_p1<ElectronContainer,ElectronContainer>; */
/* template class TTbarLLContainer_p1<ElectronContainer,Analysis::MuonContainer>; */
/* template class TTbarLLContainer_p1<ElectronContainer,Analysis::TauJetContainer>; */
/* template class TTbarLLContainer_p1<Analysis::MuonContainer,Analysis::MuonContainer>; */
/* template class TTbarLLContainer_p1<Analysis::MuonContainer,Analysis::TauJetContainer>; */
/* template class TTbarLLContainer_p1<Analysis::TauJetContainer,Analysis::TauJetContainer>; */
/* template class TTbarLLContainer_p1<TruthParticleContainer,TruthParticleContainer>; */

/* template class TTbarLLJetsContainer_p1<ElectronContainer,ElectronContainer>; */
/* template class TTbarLLJetsContainer_p1<ElectronContainer,Analysis::MuonContainer>; */
/* template class TTbarLLJetsContainer_p1<ElectronContainer,Analysis::TauJetContainer>; */
/* template class TTbarLLJetsContainer_p1<Analysis::MuonContainer,Analysis::MuonContainer>; */
/* template class TTbarLLJetsContainer_p1<Analysis::MuonContainer,Analysis::TauJetContainer>; */
/* template class TTbarLLJetsContainer_p1<Analysis::TauJetContainer,Analysis::TauJetContainer>; */
/* template class TTbarLLJetsContainer_p1<TruthParticleContainer,TruthParticleContainer>; */
