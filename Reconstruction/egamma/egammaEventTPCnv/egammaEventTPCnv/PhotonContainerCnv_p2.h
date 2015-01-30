///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonContainerCnv_p2.h 
// Header file for class PhotonContainerCnv_p2
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P2_H 
#define EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P2_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/PhotonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/PhotonContainer_p2.h"
#include "egammaEventTPCnv/PhotonCnv_p2.h"

typedef T_AthenaPoolTPCnvVector< 
            PhotonContainer, 
            PhotonContainer_p2, 
            PhotonCnv_p2 
       > PhotonContainerCnv_p2;

#endif //> EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P2_H
