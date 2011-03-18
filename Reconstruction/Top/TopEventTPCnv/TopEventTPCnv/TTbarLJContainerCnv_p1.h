/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLJCONTAINERCNV_P1_H
#define TTBARLJCONTAINERCNV_P1_H

#include "TopEvent/TTbarLJContainer.h"
#include "TopEventTPCnv/TTbarLJCnv_p1.h"
#include "TopEventTPCnv/TTbarLJContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "JetEvent/JetCollection.h"
#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarEJContainer,
		     TTbarEJContainer_p1,
		     TTbarLJCnv_p1<ElectronContainer> >         TTbarEJContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuJContainer,
		     TTbarMuJContainer_p1,
		     TTbarLJCnv_p1<Analysis::MuonContainer> >   TTbarMuJContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTauJContainer,
		     TTbarTauJContainer_p1,
		     TTbarLJCnv_p1<Analysis::TauJetContainer> > TTbarTauJContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTruthJContainer,
		     TTbarTruthJContainer_p1,
		     TTbarLJCnv_p1<TruthParticleContainer> >    TTbarTruthJContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarJeJContainer,
		     TTbarJeJContainer_p1,
		     TTbarLJCnv_p1<JetCollection> >    TTbarJeJContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarEJContainer,TTbarEJContainer_p1> : 
  public TTbarEJContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuJContainer,TTbarMuJContainer_p1> : 
  public TTbarMuJContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTauJContainer,TTbarTauJContainer_p1> : 
  public TTbarTauJContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTruthJContainer,TTbarTruthJContainer_p1> : 
  public TTbarTruthJContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarJeJContainer,TTbarJeJContainer_p1> : 
  public TTbarJeJContainerCnv_p1 
{
public:
};

#endif //TTBARLJCONTAINERCNV_P1_H
