///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaContainerCnv_p5.h 
// Header file for class egammaContainerCnv_p5
// New cnv to include Ringer: W.S.Freund<wsfreund@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P5_H 
#define EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P5_H 1

// STL includes

// Gaudi includes

// egammaEvent includes
#include "egammaEvent/egammaContainer.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egammaContainer_p5.h"
#include "egammaEventTPCnv/egammaCnv_p5.h"

typedef T_AthenaPoolTPCnvVector< 
            egammaContainer, 
            egammaContainer_p5, 
            egammaCnv_p5 
       > egammaContainerCnv_p5;

#endif //> EGAMMAEVENTTPCNV_EGAMMACONTAINERCNV_P5_H
