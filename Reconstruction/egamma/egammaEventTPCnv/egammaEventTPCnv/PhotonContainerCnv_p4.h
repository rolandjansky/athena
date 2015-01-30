///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonContainerCnv_p4.h 
// Header file for class PhotonContainerCnv_p4
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P4_H 
#define EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P4_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/PhotonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/PhotonContainer_p4.h"
#include "egammaEventTPCnv/PhotonCnv_p4.h"

typedef T_AthenaPoolTPCnvVector< 
            PhotonContainer, 
            PhotonContainer_p4, 
            PhotonCnv_p4 
       > PhotonContainerCnv_p4;

#endif //> EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P4_H
