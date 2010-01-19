/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************************

PhotonLinks.h is needed to be able to add ParticleLinks<PhotonContainer>
                    to store gate. A similar header is needed for every instantiation
                    of ParticleLinks

         S. Protopopescu  6-MAY-2009


*****************************************************************************************/

#ifndef PHOTONLINKS_H
#define PHOTONLINKS_H

#include "egammaEvent/PhotonContainer.h"
#include "ParticleEvent/ParticleLinks.h"
#include "SGTools/CLASS_DEF.h"

typedef  ParticleLinks<PhotonContainer> PhotonLinks ;
typedef  ElementLink<PhotonContainer> PhotonLink ;

CLASS_DEF(PhotonLinks,141119118 , 1)


#endif // PHOTONLINKS_H
