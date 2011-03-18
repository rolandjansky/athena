/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLLJETSMETCONTAINERCNV_P1_H
#define TTBARLLJETSMETCONTAINERCNV_P1_H

#include "TopEvent/TTbarLLJetsMetContainer.h"

#include "TopEventTPCnv/TTbarLLJetsMetCnv_p1.h"
#include "TopEventTPCnv/TTbarLLJetsMetContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "Particle/TrackParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarEEJetsMetContainer,
		     TTbarEEJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<ElectronContainer,ElectronContainer> >                 TTbarEEJetsMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarEMuJetsMetContainer,
		     TTbarEMuJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<ElectronContainer,Analysis::MuonContainer> >           TTbarEMuJetsMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarETauJetsMetContainer,
		     TTbarETauJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<ElectronContainer,Analysis::TauJetContainer> >         TTbarETauJetsMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarETrackJetsMetContainer,
		     TTbarETrackJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<ElectronContainer,Rec::TrackParticleContainer> >       TTbarETrackJetsMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuMuJetsMetContainer,
		     TTbarMuMuJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<Analysis::MuonContainer,Analysis::MuonContainer> >     TTbarMuMuJetsMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuTauJetsMetContainer,
		     TTbarMuTauJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<Analysis::MuonContainer,Analysis::TauJetContainer> >   TTbarMuTauJetsMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuTrackJetsMetContainer,
		     TTbarMuTrackJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<Analysis::MuonContainer,Rec::TrackParticleContainer> > TTbarMuTrackJetsMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTauTauJetsMetContainer,
		     TTbarTauTauJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<Analysis::TauJetContainer,Analysis::TauJetContainer> > TTbarTauTauJetsMetContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTruthTruthJetsMetContainer,
		     TTbarTruthTruthJetsMetContainer_p1,
		     TTbarLLJetsMetCnv_p1<TruthParticleContainer,TruthParticleContainer> >       TTbarTruthTruthJetsMetContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarEEJetsMetContainer,TTbarEEJetsMetContainer_p1> : 
public TTbarEEJetsMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarEMuJetsMetContainer,TTbarEMuJetsMetContainer_p1> : 
public TTbarEMuJetsMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarETauJetsMetContainer,TTbarETauJetsMetContainer_p1> : 
public TTbarETauJetsMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarETrackJetsMetContainer,TTbarETrackJetsMetContainer_p1> : 
public TTbarETrackJetsMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuMuJetsMetContainer,TTbarMuMuJetsMetContainer_p1> : 
public TTbarMuMuJetsMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuTauJetsMetContainer,TTbarMuTauJetsMetContainer_p1> : 
public TTbarMuTauJetsMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuTrackJetsMetContainer,TTbarMuTrackJetsMetContainer_p1> : 
public TTbarMuTrackJetsMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTauTauJetsMetContainer,TTbarTauTauJetsMetContainer_p1> : 
public TTbarTauTauJetsMetContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTruthTruthJetsMetContainer,TTbarTruthTruthJetsMetContainer_p1> : 
public TTbarTruthTruthJetsMetContainerCnv_p1 
{
public:
};

#endif //TTBARLLJETSMETCONTAINERCNV_P1_H
