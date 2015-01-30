///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainerCnv_p1.h 
// Header file for class egammaContainerCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P1_H 
#define EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/egammaContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaContainer_p1.h"
#include "egammaEventTPCnv/egammaCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< 
            egammaContainer, 
            egammaContainer_p1, 
            egammaCnv_p1 
       > egammaContainerCnv_p1;

#endif //> EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P1_H
