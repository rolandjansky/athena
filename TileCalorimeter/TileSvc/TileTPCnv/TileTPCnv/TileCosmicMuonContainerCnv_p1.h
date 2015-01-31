///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonContainerCnv_p1.h 
// Header file for class TileCosmicMuonContainerCnv_p1
// Author: Aranzazu Ruiz <aranzazu.ruiz.martinez@cern.ch>
// Date:   July 2008
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILECOSMICMUONCONTAINERCNV_P1_H
#define TILETPCNV_TILECOSMICMUONCONTAINERCNV_P1_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileCosmicMuonContainer_p1.h"
#include "TileTPCnv/TileCosmicMuonCnv_p1.h"

// TileEvent includes
#include "TileEvent/TileCosmicMuonContainer.h"

typedef T_AthenaPoolTPCnvVector<
            TileCosmicMuonContainer,
            TileCosmicMuonContainer_p1,
            TileCosmicMuonCnv_p1
       > TileCosmicMuonContainerCnv_p1;

#endif //> TILETPCNV_TILECOSMICMUONCONTAINERCNV_P1_H
