///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McParticleEventDict.h 
// Header for dict. 'bootstrapping'
// Author  : Sebastien Binet
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENT_MCPARTICLEEVENTDICT_H
#define MCPARTICLEEVENT_MCPARTICLEEVENTDICT_H

#include "Navigation/Navigable.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthEtIsolationsContainer.h"
#include "McParticleEvent/PileUpClassification.h"

namespace McParticleEventDict{

  struct temp {

    Navigable<TruthParticleContainer,double> m_navTruth;
    ElementLink<TruthParticleContainer> m_linkTruth;
    std::pair<ElementLink<TruthParticleContainer>,double> m_pairTruth;

    ElementLink<McEventCollection> m_genEventLink;

    DataVector<TruthParticle::TruthParticleImpl_t> m_dv;

    std::vector<TruthParticle*> m_truthParticles;
    std::vector<TruthEtIsolations*> m_truthEtIsolations;
    PileUpClassification p;
 };


} // end McParticleEventDict namespace


#endif // MCPARTICLEEVENT_MCPARTICLEEVENTDICT_H
