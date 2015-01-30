///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronContainerCnv_p4.h 
// Header file for class ElectronContainerCnv_p4
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P4_H 
#define EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P4_H 1

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronContainer_p4.h"
#include "egammaEventTPCnv/ElectronCnv_p4.h"

// egammaEvent includes
#include "egammaEvent/ElectronContainer.h"

typedef T_AthenaPoolTPCnvVector< 
            ElectronContainer, 
            ElectronContainer_p4, 
            ElectronCnv_p4 
       > ElectronContainerCnv_p4;

#endif //> EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P4_H
