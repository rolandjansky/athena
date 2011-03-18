/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef TOPJETLEPTONICCONTAINERCNV_P1_H
#define TOPJETLEPTONICCONTAINERCNV_P1_H

#include "TopEvent/TopJetLeptonicContainer.h"
#include "TopEventTPCnv/TopJetLeptonicCnv_p1.h"
#include "TopEventTPCnv/TopJetLeptonicContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TopJetElectronContainer,
		     TopJetElectronContainer_p1,
		     TopJetLeptonicCnv_p1<ElectronContainer> >         TopJetElectronContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TopJetMuonContainer,
		     TopJetMuonContainer_p1,
		     TopJetLeptonicCnv_p1<Analysis::MuonContainer> >   TopJetMuonContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TopJetTauContainer,
		     TopJetTauContainer_p1,
		     TopJetLeptonicCnv_p1<Analysis::TauJetContainer> > TopJetTauContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TopJetTruthContainer,
		     TopJetTruthContainer_p1,
		     TopJetLeptonicCnv_p1<TruthParticleContainer> >    TopJetTruthContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TopJetElectronContainer,TopJetElectronContainer_p1> : 
public TopJetElectronContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TopJetMuonContainer,TopJetMuonContainer_p1> : 
public TopJetMuonContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TopJetTauContainer,TopJetTauContainer_p1> : 
public TopJetTauContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TopJetTruthContainer,TopJetTruthContainer_p1> : 
public TopJetTruthContainerCnv_p1 
{
public:
};

#endif //TOPJETLEPTONICCONTAINERCNV_P1_H
