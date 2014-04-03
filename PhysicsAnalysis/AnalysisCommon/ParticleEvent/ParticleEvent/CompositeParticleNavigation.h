///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleNavigation.h 
// Header file for class CompositeParticleNavigation
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENT_COMPOSITEPARTICLENAVIGATION_H 
#define PARTICLEEVENT_COMPOSITEPARTICLENAVIGATION_H 

// Navigation & Event includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "Navigation/Navigable.h"

typedef Navigable<INavigable4MomentumCollection, double> CompositeParticleNavigation;

#endif //> PARTICLEEVENT_COMPOSITEPARTICLENAVIGATION_H
