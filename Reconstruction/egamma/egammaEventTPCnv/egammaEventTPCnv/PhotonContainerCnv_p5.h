///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonContainerCnv_p5.h 
// Header file for class PhotonContainerCnv_p5
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P5_H 
#define EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P5_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/PhotonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/PhotonContainer_p5.h"
#include "egammaEventTPCnv/PhotonCnv_p5.h"

typedef T_AthenaPoolTPCnvVector< 
            PhotonContainer, 
            PhotonContainer_p5, 
            PhotonCnv_p5 
       > PhotonContainerCnv_p5;

#endif //> EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P5_H
