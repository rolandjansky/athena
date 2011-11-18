///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthEtIsolationsContainerCnv_p1.h 
// Header file for class TruthEtIsolationsContainerCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCONTAINERCNV_P1_H 
#define MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// McParticleEvent includes
#include "McParticleEvent/TruthEtIsolationsContainer.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthEtIsolationsContainer_p1.h"
#include "McParticleEventTPCnv/TruthEtIsolationsCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< 
            TruthEtIsolationsContainer, 
            TruthEtIsolationsContainer_p1, 
            TruthEtIsolationsCnv_p1 
       > TruthEtIsolationsContainerCnv_p1;

// template <>
// class T_TPCnv<TruthEtIsolationsContainer, TruthEtIsolationsContainer_p1 >
//   : public TruthEtIsolationsContainerCnv_p1
// {
// public:
// };

#endif //> MCPARTICLEEVENTTPCNV_TRUTHETISOLATIONSCONTAINERCNV_P1_H
