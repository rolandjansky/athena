///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneContainerCnv.h 
// Header file for class ParticleShallowCloneContainerCnv
// Author: Karsten Koeneke - February 2013
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_PARTICLESHALLOWCLONECONTAINERCNV_H 
#define PARTICLEEVENTATHENAPOOL_PARTICLESHALLOWCLONECONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleShallowCloneContainerCnv_p1.h"

// ParticleEvent includes
#include "ParticleEvent/ParticleShallowCloneContainer.h"


typedef T_AthenaPoolTPCnvCnv< ParticleShallowCloneContainer,
                              ParticleShallowCloneContainerCnv_p1 >
  ParticleShallowCloneContainerCnv;


#endif //> PARTICLEEVENTATHENAPOOL_COMPOSITEPARTICLECONTAINERCNV_H
