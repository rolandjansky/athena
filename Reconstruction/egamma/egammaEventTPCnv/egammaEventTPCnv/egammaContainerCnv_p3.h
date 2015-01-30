///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainerCnv_p3.h 
// Header file for class egammaContainerCnv_p3
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P3_H 
#define EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P3_H 1

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/egammaContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaContainer_p3.h"
#include "egammaEventTPCnv/egammaCnv_p3.h"

typedef T_AthenaPoolTPCnvVector< 
            egammaContainer, 
            egammaContainer_p3, 
            egammaCnv_p3 
       > egammaContainerCnv_p3;

#endif //> EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P3_H
