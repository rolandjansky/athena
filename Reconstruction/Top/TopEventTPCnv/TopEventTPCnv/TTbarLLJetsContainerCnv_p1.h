/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLLJETSCONTAINERCNV_P1_H
#define TTBARLLJETSCONTAINERCNV_P1_H

#include "TopEvent/TTbarLLJetsContainer.h"
#include "TopEventTPCnv/TTbarLLJetsCnv_p1.h"
#include "TopEventTPCnv/TTbarLLJetsContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "Particle/TrackParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarEEJetsContainer,
		     TTbarEEJetsContainer_p1,
		     TTbarLLJetsCnv_p1<ElectronContainer,ElectronContainer> >                 TTbarEEJetsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarEMuJetsContainer,
		     TTbarEMuJetsContainer_p1,
		     TTbarLLJetsCnv_p1<ElectronContainer,Analysis::MuonContainer> >           TTbarEMuJetsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarETauJetsContainer,
		     TTbarETauJetsContainer_p1,
		     TTbarLLJetsCnv_p1<ElectronContainer,Analysis::TauJetContainer> >         TTbarETauJetsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarETrackJetsContainer,
		     TTbarETrackJetsContainer_p1,
		     TTbarLLJetsCnv_p1<ElectronContainer,Rec::TrackParticleContainer> >       TTbarETrackJetsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuMuJetsContainer,
		     TTbarMuMuJetsContainer_p1,
		     TTbarLLJetsCnv_p1<Analysis::MuonContainer,Analysis::MuonContainer> >     TTbarMuMuJetsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuTauJetsContainer,
		     TTbarMuTauJetsContainer_p1,
		     TTbarLLJetsCnv_p1<Analysis::MuonContainer,Analysis::TauJetContainer> >   TTbarMuTauJetsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuTrackJetsContainer,
		     TTbarMuTrackJetsContainer_p1,
		     TTbarLLJetsCnv_p1<Analysis::MuonContainer,Rec::TrackParticleContainer> > TTbarMuTrackJetsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTauTauJetsContainer,
		     TTbarTauTauJetsContainer_p1,
		     TTbarLLJetsCnv_p1<Analysis::TauJetContainer,Analysis::TauJetContainer> > TTbarTauTauJetsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTruthTruthJetsContainer,
		     TTbarTruthTruthJetsContainer_p1,
		     TTbarLLJetsCnv_p1<TruthParticleContainer,TruthParticleContainer> >       TTbarTruthTruthJetsContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarEEJetsContainer,TTbarEEJetsContainer_p1> : 
public TTbarEEJetsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarEMuJetsContainer,TTbarEMuJetsContainer_p1> : 
public TTbarEMuJetsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarETauJetsContainer,TTbarETauJetsContainer_p1> : 
public TTbarETauJetsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarETrackJetsContainer,TTbarETrackJetsContainer_p1> : 
public TTbarETrackJetsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuMuJetsContainer,TTbarMuMuJetsContainer_p1> : 
public TTbarMuMuJetsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuTauJetsContainer,TTbarMuTauJetsContainer_p1> : 
public TTbarMuTauJetsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuTrackJetsContainer,TTbarMuTrackJetsContainer_p1> : 
public TTbarMuTrackJetsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTauTauJetsContainer,TTbarTauTauJetsContainer_p1> : 
public TTbarTauTauJetsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTruthTruthJetsContainer,TTbarTruthTruthJetsContainer_p1> : 
public TTbarTruthTruthJetsContainerCnv_p1 
{
public:
};

#endif //TTBARLLJETSCONTAINERCNV_P1_H
