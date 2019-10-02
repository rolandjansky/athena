///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoContainerCnv.h 
// Header file for class NeutrinoContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTATHENAPOOL_NEUTRINOCONTAINERCNV_H 
#define PARTICLEEVENTATHENAPOOL_NEUTRINOCONTAINERCNV_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

// ParticleEvent includes
#include "ParticleEvent/NeutrinoContainer.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/NeutrinoContainerCnv_p1.h"
#include "ParticleEventTPCnv/NeutrinoContainerCnv_p2.h"


typedef T_AthenaPoolTPCnvCnv< NeutrinoContainer,
                              NeutrinoContainerCnv_p2,
                              NeutrinoContainerCnv_p1,
                              T_TPCnvNull<NeutrinoContainer> >
 NeutrinoContainerCnv;


#endif //> PARTICLEEVENTATHENAPOOL_NEUTRINOCONTAINERCNV_H
