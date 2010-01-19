/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

TruthParticleLinks.h is needed to be able to add ParticleLinks<TruthParticleContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

         S. Protopopescu  25-MAY-2009


*****************************************************************************************/

#ifndef TruthParticleLinks_H
#define TruthParticleLinks_H

#include "McParticleEvent/TruthParticleContainer.h"
#include "ParticleEvent/ParticleLinks.h"
#include "SGTools/CLASS_DEF.h"

typedef  ParticleLinks<TruthParticleContainer> TruthParticleLinks ;
typedef  ElementLink<TruthParticleContainer> TruthParticleLink ;

CLASS_DEF(TruthParticleLinks,244749662 , 1)


#endif // TruthParticleLinks_H
