///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ElectronContainerCnv_p1.h 
// Header file for class ElectronContainerCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P1_H 
#define EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/ElectronContainer_p1.h"
#include "egammaEventTPCnv/ElectronCnv_p1.h"

// egammaEvent includes
#include "egammaEvent/ElectronContainer.h"

typedef T_AthenaPoolTPCnvVector< 
            ElectronContainer, 
            ElectronContainer_p1, 
            ElectronCnv_p1 
       > ElectronContainerCnv_p1;

#endif //> EGAMMAEVENTTPCNV_ELECTRONCONTAINERCNV_P1_H
