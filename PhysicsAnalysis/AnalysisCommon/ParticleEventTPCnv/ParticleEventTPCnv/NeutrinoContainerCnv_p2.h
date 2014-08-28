///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoContainerCnv_p2.h 
// Header file for class NeutrinoContainerCnv_p2
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_NEUTRINOCONTAINERCNV_P2_H 
#define PARTICLEEVENTTPCNV_NEUTRINOCONTAINERCNV_P2_H 1

// STL includes

// Gaudi includes

// ParticleEvent includes
#include "ParticleEvent/NeutrinoContainer.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/NeutrinoContainer_p2.h"
#include "ParticleEventTPCnv/NeutrinoCnv_p2.h"

// Forward declaration
class MsgStream;

typedef T_AthenaPoolTPCnvVector<
            NeutrinoContainer,
            NeutrinoContainer_p2,
            NeutrinoCnv_p2
        > NeutrinoContainerCnv_p2;

#endif //> PARTICLEEVENTTPCNV_NEUTRINOCONTAINERCNV_P2_H
