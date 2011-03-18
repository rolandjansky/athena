/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLJDITOPSFITTEDCONTAINERCNV_P1_H
#define TTBARLJDITOPSFITTEDCONTAINERCNV_P1_H

#include "TopEvent/TTbarLJDiTopsFittedContainer.h"
#include "TopEventTPCnv/TTbarLJDiTopsFittedCnv_p1.h"
#include "TopEventTPCnv/TTbarLJDiTopsFittedContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "JetEvent/JetCollection.h"
#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarEJDiTopsFittedContainer,
		     TTbarEJDiTopsFittedContainer_p1,
		     TTbarLJDiTopsFittedCnv_p1<ElectronContainer> >         TTbarEJDiTopsFittedContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuJDiTopsFittedContainer,
		     TTbarMuJDiTopsFittedContainer_p1,
		     TTbarLJDiTopsFittedCnv_p1<Analysis::MuonContainer> >   TTbarMuJDiTopsFittedContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTauJDiTopsFittedContainer,
		     TTbarTauJDiTopsFittedContainer_p1,
		     TTbarLJDiTopsFittedCnv_p1<Analysis::TauJetContainer> > TTbarTauJDiTopsFittedContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTruthJDiTopsFittedContainer,
		     TTbarTruthJDiTopsFittedContainer_p1,
		     TTbarLJDiTopsFittedCnv_p1<TruthParticleContainer> >    TTbarTruthJDiTopsFittedContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarJeJDiTopsFittedContainer,
		     TTbarJeJDiTopsFittedContainer_p1,
		     TTbarLJDiTopsFittedCnv_p1<JetCollection> >    TTbarJeJDiTopsFittedContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarEJDiTopsFittedContainer,TTbarEJDiTopsFittedContainer_p1> : 
public TTbarEJDiTopsFittedContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuJDiTopsFittedContainer,TTbarMuJDiTopsFittedContainer_p1> : 
public TTbarMuJDiTopsFittedContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTauJDiTopsFittedContainer,TTbarTauJDiTopsFittedContainer_p1> : 
public TTbarTauJDiTopsFittedContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTruthJDiTopsFittedContainer,TTbarTruthJDiTopsFittedContainer_p1> : 
public TTbarTruthJDiTopsFittedContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarJeJDiTopsFittedContainer,TTbarJeJDiTopsFittedContainer_p1> : 
public TTbarJeJDiTopsFittedContainerCnv_p1 
{
public:
};

#endif //TTBARLJDITOPSFITTEDCONTAINERCNV_P1_H
