/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLLCONTAINERCNV_P1_H
#define TTBARLLCONTAINERCNV_P1_H

#include "TopEvent/TTbarLLContainer.h"
#include "TopEventTPCnv/TTbarLLCnv_p1.h"
#include "TopEventTPCnv/TTbarLLContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "Particle/TrackParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarEEContainer,
		     TTbarEEContainer_p1,
		     TTbarLLCnv_p1<ElectronContainer,ElectronContainer> >                 TTbarEEContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarEMuContainer,
		     TTbarEMuContainer_p1,
		     TTbarLLCnv_p1<ElectronContainer,Analysis::MuonContainer> >           TTbarEMuContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarETauContainer,
		     TTbarETauContainer_p1,
		     TTbarLLCnv_p1<ElectronContainer,Analysis::TauJetContainer> >         TTbarETauContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarETrackContainer,
		     TTbarETrackContainer_p1,
		     TTbarLLCnv_p1<ElectronContainer,Rec::TrackParticleContainer> >       TTbarETrackContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuMuContainer,
		     TTbarMuMuContainer_p1,
		     TTbarLLCnv_p1<Analysis::MuonContainer,Analysis::MuonContainer> >     TTbarMuMuContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuTauContainer,
		     TTbarMuTauContainer_p1,
		     TTbarLLCnv_p1<Analysis::MuonContainer,Analysis::TauJetContainer> >   TTbarMuTauContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuTrackContainer,
		     TTbarMuTrackContainer_p1,
		     TTbarLLCnv_p1<Analysis::MuonContainer,Rec::TrackParticleContainer> > TTbarMuTrackContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTauTauContainer,
		     TTbarTauTauContainer_p1,
		     TTbarLLCnv_p1<Analysis::TauJetContainer,Analysis::TauJetContainer> > TTbarTauTauContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTruthTruthContainer,
		     TTbarTruthTruthContainer_p1,
		     TTbarLLCnv_p1<TruthParticleContainer,TruthParticleContainer> >       TTbarTruthTruthContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarEEContainer,TTbarEEContainer_p1> : 
public TTbarEEContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarEMuContainer,TTbarEMuContainer_p1> : 
public TTbarEMuContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarETauContainer,TTbarETauContainer_p1> : 
public TTbarETauContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarETrackContainer,TTbarETrackContainer_p1> : 
public TTbarETrackContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuMuContainer,TTbarMuMuContainer_p1> : 
public TTbarMuMuContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuTauContainer,TTbarMuTauContainer_p1> : 
public TTbarMuTauContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuTrackContainer,TTbarMuTrackContainer_p1> : 
public TTbarMuTrackContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTauTauContainer,TTbarTauTauContainer_p1> : 
public TTbarTauTauContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTruthTruthContainer,TTbarTruthTruthContainer_p1> : 
public TTbarTruthTruthContainerCnv_p1 
{
public:
};

#endif //TTBARLLCONTAINERCNV_P1_H
