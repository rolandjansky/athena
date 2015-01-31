///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileTTL1CellContainerCnv_p1.h 
// Header file for class TileTTL1CellContainerCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILETTL1CELLCONTAINERCNV_P1_H
#define TILETPCNV_TILETTL1CELLCONTAINERCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileTTL1CellContainer_p1.h"
#include "TileTPCnv/TileTTL1CellCnv_p1.h"

// TileEvent includes
#include "TileEvent/TileTTL1CellContainer.h"

typedef T_AthenaPoolTPCnvVector<
            TileTTL1CellContainer,
            TileTTL1CellContainer_p1,
            TileTTL1CellCnv_p1
       > TileTTL1CellContainerCnv_p1;

#endif //> TILETPCNV_TILETTL1CELLCONTAINERCNV_P1_H
