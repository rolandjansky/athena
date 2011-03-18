/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLJMETCONTAINERCNV_P1_H
#define TTBARLJMETCONTAINERCNV_P1_H

#include "TopEvent/TTbarLJMetContainer.h"
#include "TopEventTPCnv/TTbarLJMetCnv_p1.h"
#include "TopEventTPCnv/TTbarLJMetContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "JetEvent/JetCollection.h"
#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarEJMetContainer,
		     TTbarEJMetContainer_p1,
		     TTbarLJMetCnv_p1<ElectronContainer> >         TTbarEJMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuJMetContainer,
		     TTbarMuJMetContainer_p1,
		     TTbarLJMetCnv_p1<Analysis::MuonContainer> >   TTbarMuJMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTauJMetContainer,
		     TTbarTauJMetContainer_p1,
		     TTbarLJMetCnv_p1<Analysis::TauJetContainer> > TTbarTauJMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTruthJMetContainer,
		     TTbarTruthJMetContainer_p1,
		     TTbarLJMetCnv_p1<TruthParticleContainer> >    TTbarTruthJMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarJeJMetContainer,
		     TTbarJeJMetContainer_p1,
		     TTbarLJMetCnv_p1<JetCollection> >             TTbarJeJMetContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarEJMetContainer,TTbarEJMetContainer_p1> : 
public TTbarEJMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuJMetContainer,TTbarMuJMetContainer_p1> : 
public TTbarMuJMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTauJMetContainer,TTbarTauJMetContainer_p1> : 
public TTbarTauJMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTruthJMetContainer,TTbarTruthJMetContainer_p1> : 
public TTbarTruthJMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarJeJMetContainer,TTbarJeJMetContainer_p1> : 
public TTbarJeJMetContainerCnv_p1 
{
public:
};

#endif //TTBARLJMETCONTAINERCNV_P1_H
