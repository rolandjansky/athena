///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PhotonContainerCnv_p1.h 
// Header file for class PhotonContainerCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P1_H 
#define EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P1_H 

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/PhotonContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/PhotonContainer_p1.h"
#include "egammaEventTPCnv/PhotonCnv_p1.h"

typedef T_AthenaPoolTPCnvVector< 
            PhotonContainer, 
            PhotonContainer_p1, 
            PhotonCnv_p1 
       > PhotonContainerCnv_p1;

#endif //> EGAMMAEVENTTPCNV_PHOTONCONTAINERCNV_P1_H
