///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainerCnv_p2.h 
// Header file for class egammaContainerCnv_p2
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P2_H 
#define EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P2_H 1

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/egammaContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaContainer_p2.h"
#include "egammaEventTPCnv/egammaCnv_p2.h"

typedef T_AthenaPoolTPCnvVector< 
            egammaContainer, 
            egammaContainer_p2, 
            egammaCnv_p2 
       > egammaContainerCnv_p2;

#endif //> EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P2_H
