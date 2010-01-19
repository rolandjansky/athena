/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

TrackLinks.h is needed to be able to add ParticleLinks<TrackParticleContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

         S. Protopopescu  11-September-2009

*****************************************************************************************/
#ifndef TRACKLINKS_H
#define TRACKLINKS_H

#include "ParticleEvent/ParticleLinks.h"
#include "Particle/TrackParticleContainer.h"
#include "SGTools/CLASS_DEF.h"

typedef  ParticleLinks<Rec::TrackParticleContainer> TrackLinks ;
typedef  ElementLink<Rec::TrackParticleContainer> TrackLink ;


CLASS_DEF(TrackLinks,253440163 , 1)


#endif // TRACKLINKS_H
