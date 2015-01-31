///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsContainerCnv_p1.h 
// Header file for class TileDigitsContainerCnv_p1
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEDIGITSCONTAINERCNV_P1_H
#define TILETPCNV_TILEDIGITSCONTAINERCNV_P1_H

// TileTPCnv includes
#define private public
#include "TileTPCnv/TileDigitsContainer_p1.h"
#undef private 
#include "TileTPCnv/TileDigitsCnv_p1.h"
#include "TileTPCnv/T_TilePoolContainerCnv.h"

// Tile includes
#include "TileEvent/TileDigitsContainer.h"

typedef T_TilePoolContainerCnv<
            TileDigitsContainer,
            TileDigitsContainer_p1,
            TileDigitsCnv_p1
       > TileDigitsContainerCnv_p1;

#endif //> TILETPCNV_TILEDIGITSCONTAINERCNV_P1_H

