///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronContainerCnv_p3.h 
// Header file for class ElectronContainerCnv_p3
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P3_H 
#define EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P3_H 1

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronContainer_p3.h"
#include "egammaEventTPCnv/ElectronCnv_p3.h"

// egammaEvent includes
#include "egammaEvent/ElectronContainer.h"

typedef T_AthenaPoolTPCnvVector< 
            ElectronContainer, 
            ElectronContainer_p3, 
            ElectronCnv_p3 
       > ElectronContainerCnv_p3;

#endif //> EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P3_H
