// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file      ParticleEvent/IParticleLinks.h
 * @author    sss, S. Protopopescu
 * @date      Mar, 2010
 * @brief     Wrapper for a vector of links to @c IParticle, to be put in SG.
 */


#ifndef PARTICLEEVENT_IPARTICLELINKS_H
#define PARTICLEEVENT_IPARTICLELINKS_H 1

#include "NavFourMom/IParticleContainer.h"
#include "NavFourMom/IParticleLink.h"
#include "ParticleEvent/ParticleLinks.h"
#include "SGTools/CLASS_DEF.h"


/// Wrapper for a vector of links to @c IParticle, to be put in SG.
typedef  ParticleLinks<IParticleContainer> IParticleLinks ;

CLASS_DEF(IParticleLinks, 236861931, 1)


#endif // !PARTICLEEVENT_IPARTICLELINKS_H
