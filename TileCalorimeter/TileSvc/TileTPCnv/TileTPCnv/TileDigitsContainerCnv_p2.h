///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsContainerCnv_p2.h 
// Header file for class TileDigitsContainerCnv_p2
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEDIGITSCONTAINERCNV_P2_H
#define TILETPCNV_TILEDIGITSCONTAINERCNV_P2_H

// TileTPCnv includes
#define private public
#include "TileTPCnv/TileDigitsContainer_p2.h"
#undef private 
#include "TileTPCnv/TileDigitsCnv_p2.h"
#include "TileTPCnv/T_TilePoolContainerCnv.h"

// Tile includes
#include "TileEvent/TileDigitsContainer.h"

typedef T_TilePoolContainerCnv<
            TileDigitsContainer,
            TileDigitsContainer_p2,
            TileDigitsCnv_p2
       > TileDigitsContainerCnv_p2;

#endif //> TILETPCNV_TILEDIGITSCONTAINERCNV_P2_H

