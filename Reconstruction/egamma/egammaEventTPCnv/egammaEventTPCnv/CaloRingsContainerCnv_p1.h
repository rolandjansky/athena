///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloRingsContainerCnv_p1.h 
// Header file for class CaloRingsContainerCnv_p1
// Author: D.E.Ferreira de Lima<dferreir@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_CALORINGSCONTAINERCNV_P1_H 
#define RECTPCNV_CALORINGSCONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/CaloRingsContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "egammaEventTPCnv/CaloRingsContainer_p1.h"
#include "egammaEventTPCnv/CaloRingsCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< 
            CaloRingsContainer, 
            CaloRingsContainer_p1, 
            CaloRingsCnv_p1 
       > CaloRingsContainerCnv_p1;

#endif //> RECTPCNV_CALORINGSCONTAINERCNV_P1_H
