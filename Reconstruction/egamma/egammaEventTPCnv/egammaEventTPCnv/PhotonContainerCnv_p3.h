///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonContainerCnv_p3.h 
// Header file for class PhotonContainerCnv_p3
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P3_H 
#define EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P3_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/PhotonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/PhotonContainer_p3.h"
#include "egammaEventTPCnv/PhotonCnv_p3.h"

typedef T_AthenaPoolTPCnvVector< 
            PhotonContainer, 
            PhotonContainer_p3, 
            PhotonCnv_p3 
       > PhotonContainerCnv_p3;

#endif //> EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P3_H
