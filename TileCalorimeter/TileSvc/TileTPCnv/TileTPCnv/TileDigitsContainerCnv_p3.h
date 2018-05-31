///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TileDigitsContainerCnv_p3.h 
// Header file for class TileDigitsContainerCnv_p3
// Author: Alexander Solodkov <Sanya.Solodkov@cern.ch>
// Date:   June 2009
/////////////////////////////////////////////////////////////////// 
#ifndef TILETPCNV_TILEDIGITSCONTAINERCNV_P3_H
#define TILETPCNV_TILEDIGITSCONTAINERCNV_P3_H

// TileTPCnv includes
#define private public
#include "TileTPCnv/TileDigitsContainer_p3.h"
#undef private 
#include "TileTPCnv/TileDigitsCnv_p3.h"
#include "TileTPCnv/T_TilePoolContainerCnv.h"

// Tile includes
#include "TileEvent/TileDigitsContainer.h"

typedef T_TilePoolContainerCnv<
            TileDigitsContainer,
            TileDigitsContainer_p3,
            TileDigitsCnv_p3
       > TileDigitsContainerCnv_p3;

#endif //> TILETPCNV_TILEDIGITSCONTAINERCNV_P3_H

