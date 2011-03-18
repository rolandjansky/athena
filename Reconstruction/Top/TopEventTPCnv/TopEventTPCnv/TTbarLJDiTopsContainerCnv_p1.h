/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-

#ifndef TTBARLJDITOPSCONTAINERCNV_P1_H
#define TTBARLJDITOPSCONTAINERCNV_P1_H

#include "TopEvent/TTbarLJDiTopsContainer.h"
#include "TopEventTPCnv/TTbarLJDiTopsCnv_p1.h"
#include "TopEventTPCnv/TTbarLJDiTopsContainer_p1.h"

#include "TopEventTPCnv/VectorCnv_p1.h"

#include "JetEvent/JetCollection.h"
#include "egammaEvent/ElectronContainer.h"
#include "muonEvent/MuonContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "McParticleEvent/TruthParticleContainer.h"

// VectorCnv_p1
typedef VectorCnv_p1<TopRec::TTbarEJDiTopsContainer,
		     TTbarEJDiTopsContainer_p1,
		     TTbarLJDiTopsCnv_p1<ElectronContainer> >         TTbarEJDiTopsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarMuJDiTopsContainer,
		     TTbarMuJDiTopsContainer_p1,
		     TTbarLJDiTopsCnv_p1<Analysis::MuonContainer> >   TTbarMuJDiTopsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTauJDiTopsContainer,
		     TTbarTauJDiTopsContainer_p1,
		     TTbarLJDiTopsCnv_p1<Analysis::TauJetContainer> > TTbarTauJDiTopsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarTruthJDiTopsContainer,
		     TTbarTruthJDiTopsContainer_p1,
		     TTbarLJDiTopsCnv_p1<TruthParticleContainer> >    TTbarTruthJDiTopsContainerCnv_p1;

typedef VectorCnv_p1<TopRec::TTbarJeJDiTopsContainer,
		     TTbarJeJDiTopsContainer_p1,
		     TTbarLJDiTopsCnv_p1<JetCollection> >    TTbarJeJDiTopsContainerCnv_p1;

// T_TPCnv
template <> class T_TPCnv<TopRec::TTbarEJDiTopsContainer,TTbarEJDiTopsContainer_p1> : 
public TTbarEJDiTopsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarMuJDiTopsContainer,TTbarMuJDiTopsContainer_p1> : 
public TTbarMuJDiTopsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTauJDiTopsContainer,TTbarTauJDiTopsContainer_p1> : 
public TTbarTauJDiTopsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarTruthJDiTopsContainer,TTbarTruthJDiTopsContainer_p1> : 
public TTbarTruthJDiTopsContainerCnv_p1 
{
public:
};

template <> class T_TPCnv<TopRec::TTbarJeJDiTopsContainer,TTbarJeJDiTopsContainer_p1> : 
public TTbarJeJDiTopsContainerCnv_p1 
{
public:
};

#endif //TTBARLJDITOPSCONTAINERCNV_P1_H
