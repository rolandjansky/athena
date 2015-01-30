///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainerCnv_p4.h 
// Header file for class egammaContainerCnv_p4
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P4_H 
#define EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P4_H 1

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/egammaContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaContainer_p4.h"
#include "egammaEventTPCnv/egammaCnv_p4.h"

typedef T_AthenaPoolTPCnvVector< 
            egammaContainer, 
            egammaContainer_p4, 
            egammaCnv_p4 
       > egammaContainerCnv_p4;

#endif //> EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P4_H
