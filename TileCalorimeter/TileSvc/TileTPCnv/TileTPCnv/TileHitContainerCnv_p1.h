///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileHitContainerCnv_p1.h 
// Header file for class TileHitContainerCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEHITCONTAINERCNV_P1_H
#define TILETPCNV_TILEHITCONTAINERCNV_P1_H

// TileTPCnv includes
#define private public
#include "TileTPCnv/TileHitContainer_p1.h"
#undef private 
#include "TileSimEventTPCnv/TileHitCnv_p1.h"
#include "TileTPCnv/T_TilePoolContainerCnv.h"

// Tile includes
#include "TileEvent/TileHitContainer.h"

typedef T_TilePoolContainerCnv<
            TileHitContainer,
            TileHitContainer_p1,
            TileHitCnv_p1
       > TileHitContainerCnv_p1;

#endif //> TILETPCNV_TILEHITCONTAINERCNV_P1_H

