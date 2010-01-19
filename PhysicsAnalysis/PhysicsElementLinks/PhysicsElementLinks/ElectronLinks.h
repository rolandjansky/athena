/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

ElectronLinks.h is needed to be able to add ParticleLinks<ElectronContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

         S. Protopopescu  15-DEC-2008


*****************************************************************************************/

#ifndef ELECTRONLINKS_H
#define ELECTRONLINKS_H

#include "egammaEvent/ElectronContainer.h"
#include "ParticleEvent/ParticleLinks.h"
#include "SGTools/CLASS_DEF.h"

typedef  ParticleLinks<ElectronContainer> ElectronLinks ;
typedef  ElementLink<ElectronContainer> ElectronLink ;

CLASS_DEF(ElectronLinks, 107026136 , 1)


#endif // ELECTRONLINKS_H
