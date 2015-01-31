///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileCosmicMuonContainerCnv_p2.h 
// Header file for class TileCosmicMuonContainerCnv_p2
// Author: Jose Maneira <maneira@lip.pt>
// Date:   July 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILECOSMICMUONCONTAINERCNV_P2_H
#define TILETPCNV_TILECOSMICMUONCONTAINERCNV_P2_H

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// TileTPCnv includes
#include "TileTPCnv/TileCosmicMuonContainer_p2.h"
#include "TileTPCnv/TileCosmicMuonCnv_p2.h"

// TileEvent includes
#include "TileEvent/TileCosmicMuonContainer.h"

typedef T_AthenaPoolTPCnvVector<
            TileCosmicMuonContainer,
            TileCosmicMuonContainer_p2,
            TileCosmicMuonCnv_p2
       > TileCosmicMuonContainerCnv_p2;

#endif //> TILETPCNV_TILECOSMICMUONCONTAINERCNV_P2_H
