/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLEEVENT_PARTICLEBASECONTAINER_H
#define PARTICLEEVENT_PARTICLEBASECONTAINER_H


#include "ParticleEvent/ParticleBase.h"
#include "AthContainers/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/IParticleContainer.h"

//DATAVECTOR_VIRTBASES1 (ParticleBase, IParticle);

typedef  DataVector<ParticleBase> ParticleBaseContainer ;

CLASS_DEF(ParticleBaseContainer, 1725646416, 1)

  //SG_BASE(ParticleBaseContainer, DataVector<ParticleBase> ); // < needed for auto-symlink feature

#endif // !PARTICLEEVENT_PARTICLEBASECONTAINER_H
