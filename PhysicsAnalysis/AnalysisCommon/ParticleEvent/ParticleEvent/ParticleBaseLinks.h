// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

ParticleBaseLinks.h is needed to be able to add ParticleLinks<ParticleBaseContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

*****************************************************************************************/

#ifndef PARTICLEEVENT_PARTICLEBASELINKS_H
#define PARTICLEEVENT_PARTICLEBASELINKS_H 1

#include "ParticleEvent/ParticleBaseContainer.h"
#include "ParticleEvent/ParticleLinks.h"
#include "SGTools/CLASS_DEF.h"

typedef ParticleLinks<ParticleBaseContainer> ParticleBaseLinks ;
typedef ElementLink<ParticleBaseContainer> ParticleBaseLink;

CLASS_DEF(ParticleBaseLinks, 236848729, 1)


#endif // !PARTICLEEVENT_PARTICLEBASELINKS_H
