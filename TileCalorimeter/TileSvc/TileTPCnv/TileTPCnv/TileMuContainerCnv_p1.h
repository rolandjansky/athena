///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileMuContainerCnv_p1.h 
// Header file for class TileMuContainerCnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEMUCONTAINERCNV_P1_H
#define TILETPCNV_TILEMUCONTAINERCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileMuContainer_p1.h"
#include "TileTPCnv/TileMuCnv_p1.h"

// TileEvent includes
#include "TileEvent/TileMuContainer.h"

typedef T_AthenaPoolTPCnvVector<
            TileMuContainer,
            TileMuContainer_p1,
            TileMuCnv_p1
       > TileMuContainerCnv_p1;

#endif //> TILETPCNV_TILEMUCONTAINERCNV_P1_H
