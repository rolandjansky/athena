///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronContainerCnv_p2.h 
// Header file for class ElectronContainerCnv_p2
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P2_H 
#define EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P2_H 1

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronContainer_p2.h"
#include "egammaEventTPCnv/ElectronCnv_p2.h"

// egammaEvent includes
#include "egammaEvent/ElectronContainer.h"

typedef T_AthenaPoolTPCnvVector< 
            ElectronContainer, 
            ElectronContainer_p2, 
            ElectronCnv_p2 
       > ElectronContainerCnv_p2;

#endif //> EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P2_H
