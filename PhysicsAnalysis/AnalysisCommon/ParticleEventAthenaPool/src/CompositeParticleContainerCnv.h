///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleContainerCnv.h 
// Header file for class CompositeParticleContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_COMPOSITEPARTICLECONTAINERCNV_H 
#define PARTICLEEVENTATHENAPOOL_COMPOSITEPARTICLECONTAINERCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/CompositeParticleContainer_p1.h"
#include "ParticleEventTPCnv/CompositeParticleContainerCnv_p1.h"

// ParticleEvent includes
#include "ParticleEvent/CompositeParticleContainer.h"


// Forward declaration

// the latest persistent representation type of DataCollection:
typedef T_AthenaPoolTPCnvCnv<CompositeParticleContainer,
                             CompositeParticleContainerCnv_p1>
  CompositeParticleContainerCnv;


#endif //> PARTICLEEVENTATHENAPOOL_COMPOSITEPARTICLECONTAINERCNV_H
