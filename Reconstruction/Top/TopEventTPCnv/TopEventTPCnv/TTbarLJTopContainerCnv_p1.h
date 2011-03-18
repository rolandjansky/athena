/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLJTOPCONTAINERCNV_P1_H
#define TTBARLJTOPCONTAINERCNV_P1_H

#include "TopEvent/TTbarLJTopContainer.h"
#include "TopEventTPCnv/TTbarLJTopCnv_p1.h"
#include "TopEventTPCnv/TTbarLJTopContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarEJTopContainer,
		     TTbarEJTopContainer_p1,
		     TTbarLJTopCnv_p1<ElectronContainer> >         TTbarEJTopContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuJTopContainer,
		     TTbarMuJTopContainer_p1,
		     TTbarLJTopCnv_p1<Analysis::MuonContainer> >   TTbarMuJTopContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTauJTopContainer,
		     TTbarTauJTopContainer_p1,
		     TTbarLJTopCnv_p1<Analysis::TauJetContainer> > TTbarTauJTopContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTruthJTopContainer,
		     TTbarTruthJTopContainer_p1,
		     TTbarLJTopCnv_p1<TruthParticleContainer> >    TTbarTruthJTopContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarEJTopContainer,TTbarEJTopContainer_p1> : 
  public TTbarEJTopContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuJTopContainer,TTbarMuJTopContainer_p1> : 
  public TTbarMuJTopContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTauJTopContainer,TTbarTauJTopContainer_p1> : 
  public TTbarTauJTopContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTruthJTopContainer,TTbarTruthJTopContainer_p1> : 
  public TTbarTruthJTopContainerCnv_p1 
{
public:
};

#endif //TTBARLJTOPCONTAINERCNV_P1_H
