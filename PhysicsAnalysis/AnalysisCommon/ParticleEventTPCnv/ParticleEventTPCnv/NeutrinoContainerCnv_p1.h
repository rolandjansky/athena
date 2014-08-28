///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoContainerCnv_p1.h 
// Header file for class NeutrinoContainerCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_NEUTRINOCONTAINERCNV_P1_H 
#define PARTICLEEVENTTPCNV_NEUTRINOCONTAINERCNV_P1_H 1

// STL includes

// Gaudi includes

// ParticleEvent includes
#include "ParticleEvent/NeutrinoContainer.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/NeutrinoContainer_p1.h"
#include "ParticleEventTPCnv/NeutrinoCnv_p1.h"

// Forward declaration
class MsgStream;

typedef T_AthenaPoolTPCnvVector<
            NeutrinoContainer,
            NeutrinoContainer_p1,
            NeutrinoCnv_p1
        > NeutrinoContainerCnv_p1;

#endif //> PARTICLEEVENTTPCNV_NEUTRINOCONTAINERCNV_P1_H
