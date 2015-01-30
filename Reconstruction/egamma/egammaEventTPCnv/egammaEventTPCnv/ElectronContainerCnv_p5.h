///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronContainerCnv_p5.h 
// Header file for class ElectronContainerCnv_p5
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P5_H 
#define EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P5_H 1

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronContainer_p5.h"
#include "egammaEventTPCnv/ElectronCnv_p5.h"

// egammaEvent includes
#include "egammaEvent/ElectronContainer.h"

typedef T_AthenaPoolTPCnvVector< 
            ElectronContainer, 
            ElectronContainer_p5, 
            ElectronCnv_p5 
       > ElectronContainerCnv_p5;

#endif //> EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P5_H
